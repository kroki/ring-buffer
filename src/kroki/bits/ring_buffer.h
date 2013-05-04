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

#ifndef KROKI_BITS_RING_BUFFER_H
#define KROKI_BITS_RING_BUFFER_H 1


struct kroki_ring_buffer;


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


__attribute__((__nothrow__))
struct kroki_ring_buffer *
kroki_ring_buffer_open(const char *filename, unsigned int capacity);


__attribute__((__nothrow__))
unsigned int
kroki_ring_buffer_get_capacity(struct kroki_ring_buffer *rb);


__attribute__((__nothrow__))
void *
kroki_ring_buffer_get_free(struct kroki_ring_buffer *rb, unsigned int *size);


__attribute__((__nothrow__))
void
kroki_ring_buffer_add(struct kroki_ring_buffer *rb, unsigned int size);


__attribute__((__nothrow__))
void *
kroki_ring_buffer_get_data(struct kroki_ring_buffer *rb, unsigned int *size);


__attribute__((__nothrow__))
void
kroki_ring_buffer_del(struct kroki_ring_buffer *rb, unsigned int size);


__attribute__((__nothrow__))
void
kroki_ring_buffer_close(struct kroki_ring_buffer *rb);


#ifdef __cplusplus
}      /* extern "C" */
#endif  /* __cplusplus */


#endif  /* ! KROKI_BITS_RING_BUFFER_H */
