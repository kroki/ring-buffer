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


  DESCRIPTION:

  Ring buffer for inter-process communication.

  Kroki/ring-buffer implements ring buffer that assumes one writer and
  one reader process working concurrently (when there are several
  readers or several writers all readers and all writers should
  serialize access to ring buffer so that at most one reader and one
  writer are active at any given moment).

  Normally the writer calls ring_buffer_open() with non-zero capacity.
  After that it performs writes by first obtaining free space pointer
  and size with ring_buffer_get_free(), filling in some data, and
  making it available for reading with ring_buffer_add().

  The reader opens ring buffer with ring_buffer_open() passing zero
  capacity (meaning it accepts whatever capacity it has).  Then it
  obtains a pointer to available data and its size with
  ring_buffer_get_data(), processes (some part of) it and releases
  processed amount with ring_buffer_del().

  Both reader and writer should call ring_buffer_close() when done to
  release associated resources.

  Implementation is immune to process crashes provided that a crashed
  process is restarted and reopens the buffer with ring_buffer_open().
  No deadlock will arise and ring_buffer_add()/ring_buffer_del()
  transactions will be preserved (uncommitted data will be lost).
  However after host crash ring buffer file may have undefined
  contents and should be removed (so it is best to put ring buffer
  files to some in-memory file system).


    struct ring_buffer *
    ring_buffer_open(const char *filename, unsigned int capacity);

      Open ring buffer file 'filename' and return ring buffer handle.
      If the file didn't exist it is created with a given 'capacity'
      (rounded upward to page size) and read-write permissions for all
      (subject to process umask).  If the file did exist its original
      capacity is not changed.

      When 'capacity' is zero the call will block until ring buffer is
      initialized with non-zero capacity by another process.

      When file can't be opened or created NULL is returned and
      'errno' is set.


    unsigned int
    ring_buffer_get_capacity(struct ring_buffer *rb);

      Return ring buffer capacity.


    void *
    ring_buffer_get_free(struct ring_buffer *rb, unsigned int *size);

      Return pointer to the beginning of and update 'size' to the size
      of the free space in the ring buffer.  Free space is always
      continuous.  When '*size' is zero the call is non-blocking,
      otherwise the call blocks until at least '*size' free bytes are
      available.


    void
    ring_buffer_add(struct ring_buffer *rb, unsigned int size);

      Atomically mark first 'size' bytes of free space as now having
      data available for reading.


    void *
    ring_buffer_get_data(struct ring_buffer *rb, unsigned int *size);

      Return pointer to the beginning of and update 'size' to the size
      of the data space in the ring buffer.  Data space is always
      continuous.  When '*size' is zero the call is non-blocking,
      otherwise the call blocks until at least '*size' data bytes are
      available.


    void
    ring_buffer_del(struct ring_buffer *rb, unsigned int size);

      Atomically mark first 'size' bytes of data as now belonging to
      free space available for writing.


    void
    ring_buffer_close(struct ring_buffer *rb);

      Free all ring buffer resources.


  Defining KROKI_RING_BUFFER_NOPOLLUTE will result in omitting alias
  definitions, but functionality will still be available with the
  namespace prefix 'kroki_'.

  Implementation requires Linux kernel, GCC 4.7.3+ and Glibc.
*/

#ifndef KROKI_RING_BUFFER_NOPOLLUTE

#define ring_buffer                             \
  kroki_ring_buffer

#define ring_buffer_open(filename, capacity)    \
  kroki_ring_buffer_open(filename, capacity)

#define ring_buffer_get_capacity(rb)            \
  kroki_ring_buffer_get_capacity(rb)

#define ring_buffer_get_free(rb, psize)         \
  kroki_ring_buffer_get_free(rb, psize)

#define ring_buffer_add(rb, size)               \
  kroki_ring_buffer_add(rb, size)

#define ring_buffer_get_data(rb, psize)         \
  kroki_ring_buffer_get_data(rb, psize)

#define ring_buffer_del(rb, size)               \
  kroki_ring_buffer_del(rb, size)

#define ring_buffer_close(rb)                   \
  kroki_ring_buffer_close(rb)

#endif  /* ! KROKI_RING_BUFFER_NOPOLLUTE */


#ifndef KROKI_RING_BUFFER_H
#define KROKI_RING_BUFFER_H 1

#include "bits/ring_buffer.h"

#endif  /* ! KROKI_RING_BUFFER_H */
