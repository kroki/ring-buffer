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
