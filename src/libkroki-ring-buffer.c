/*
  Copyright (C) 2013 Tomash Brechko.  All rights reserved.

  This file is part of kroki/ring-buffer.

  Kroki/ring-buffer is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Kroki/ring-buffer is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with kroki/ring-buffer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "kroki/bits/ring_buffer.h"
#include <kroki/error.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <linux/futex.h>
#include <sys/syscall.h>


#define MMAP(mmap)  CHECK(mmap, == MAP_FAILED, die, "%m")


struct kroki_ring_buffer
{
  unsigned int capacity;
  unsigned int produced;
  unsigned int produced_waited;
  unsigned int consumed;
  unsigned int consumed_waited;
};


static unsigned int page_size;


static __attribute__((__constructor__))
void
init(void)
{
  page_size = SYS(sysconf(_SC_PAGESIZE));
}


static inline
unsigned int
round_page_up(unsigned int size)
{
  unsigned int page_mask = page_size - 1;
  return ((size + page_mask) & ~page_mask);
}


static inline
int
futex_wait(unsigned int *uaddr, unsigned int val)
{
  int res = RESTART(syscall(SYS_futex, (int *) uaddr, FUTEX_WAIT, val,
                            NULL, NULL, 0));
  if (res == -1 && errno == EWOULDBLOCK)
    res = 0;

  return res;
}


static inline
int
futex_wake(unsigned int *uaddr)
{
  return syscall(SYS_futex, (int *) uaddr, FUTEX_WAKE, 0x7fffffff,
                 NULL, NULL, 0);
}


struct kroki_ring_buffer *
kroki_ring_buffer_open(const char *filename, unsigned int capacity)
{
  int fd = open(filename, O_RDWR | O_CREAT | O_CLOEXEC,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (fd == -1)
    return NULL;

  capacity = round_page_up(capacity);

  /*
    Enlarge the file to one page if it is new, but do not shrink if it
    already has non-zero size.
  */
  POSIX(posix_fallocate(fd, 0, page_size));

  struct kroki_ring_buffer *rb =
    MMAP(mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

  if (capacity)
    {
      unsigned int current_capacity = 0;
      if (! __atomic_compare_exchange_n(&rb->capacity,
                                        &current_capacity, capacity,
                                        0, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
        {
          capacity = current_capacity;
        }
    }
  else
    {
      capacity = __atomic_load_n(&rb->capacity, __ATOMIC_RELAXED);
      if (! capacity)
        {
          SYS(futex_wait(&rb->capacity, 0));
          capacity = __atomic_load_n(&rb->capacity, __ATOMIC_RELAXED);
        }
    }

  SYS(futex_wake(&rb->capacity));

  /*
    Since updating produced and consumed counters and signaling with
    futex_wake() is not atomic it's possible that a process will die
    after performing the update but before signaling the event.  Here
    we unfreeze possible waiters.
  */
  if (__atomic_load_n(&rb->produced_waited, __ATOMIC_RELAXED))
    SYS(futex_wake(&rb->produced));
  if (__atomic_load_n(&rb->consumed_waited, __ATOMIC_RELAXED))
    SYS(futex_wake(&rb->consumed));

  SYS(munmap(rb, page_size));

  /*
    At this point all processes agree on buffer capacity so
    ftruncate() below either sets the file size or is a no-op.
  */
  SYS(ftruncate(fd, page_size + capacity));

  /*
    Map the buffer twice for straightforward handling of wrap around
    as described in

      http://en.wikipedia.org/wiki/Circular_buffer#Optimization
  */
  rb = MMAP(mmap(NULL, page_size + capacity * 2,
                 PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
  MMAP(mmap((char *) rb, page_size + capacity,
            PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0));
  MMAP(mmap((char *) rb + page_size + capacity, capacity,
            PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, page_size));

  SYS(close(fd));

  return rb;
}


unsigned int
kroki_ring_buffer_get_capacity(struct kroki_ring_buffer *rb)
{
  return rb->capacity;
}


void *
kroki_ring_buffer_get_free(struct kroki_ring_buffer *rb, unsigned int *size)
{
  unsigned int consumed = __atomic_load_n(&rb->consumed, __ATOMIC_ACQUIRE);
  while (*size > rb->capacity - (rb->produced - consumed))
    {
      __atomic_store_n(&rb->consumed_waited, 1, __ATOMIC_RELAXED);
      SYS(futex_wait(&rb->consumed, consumed));
      __atomic_store_n(&rb->consumed_waited, 0, __ATOMIC_RELAXED);

      consumed = __atomic_load_n(&rb->consumed, __ATOMIC_ACQUIRE);
    }

  *size = rb->capacity - (rb->produced - consumed);

  return ((char *) rb + page_size + rb->produced % rb->capacity);
}


void
kroki_ring_buffer_add(struct kroki_ring_buffer *rb, unsigned int size)
{
  unsigned int old_produced = rb->produced;
  __atomic_store_n(&rb->produced, old_produced + size, __ATOMIC_RELEASE);

  unsigned int consumed = __atomic_load_n(&rb->consumed, __ATOMIC_RELAXED);
  if (__atomic_load_n(&rb->produced_waited, __ATOMIC_RELAXED))
    SYS(futex_wake(&rb->produced));
}


void *
kroki_ring_buffer_get_data(struct kroki_ring_buffer *rb, unsigned int *size)
{
  unsigned int produced = __atomic_load_n(&rb->produced, __ATOMIC_ACQUIRE);
  while (*size > produced - rb->consumed)
    {
      __atomic_store_n(&rb->produced_waited, 1, __ATOMIC_RELAXED);
      SYS(futex_wait(&rb->produced, produced));
      __atomic_store_n(&rb->produced_waited, 0, __ATOMIC_RELAXED);

      produced = __atomic_load_n(&rb->produced, __ATOMIC_ACQUIRE);
    }

  *size = produced - rb->consumed;

  return ((char *) rb + page_size + rb->consumed % rb->capacity);
}


void
kroki_ring_buffer_del(struct kroki_ring_buffer *rb, unsigned int size)
{
  unsigned int old_consumed = rb->consumed;
  __atomic_store_n(&rb->consumed, old_consumed + size, __ATOMIC_RELEASE);

  unsigned int produced = __atomic_load_n(&rb->produced, __ATOMIC_RELAXED);
  if (__atomic_load_n(&rb->consumed_waited, __ATOMIC_RELAXED))
    SYS(futex_wake(&rb->consumed));
}


void
kroki_ring_buffer_close(struct kroki_ring_buffer *rb)
{
  SYS(munmap(rb, page_size + rb->capacity * 2));
}
