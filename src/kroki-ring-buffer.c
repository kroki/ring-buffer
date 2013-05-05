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
#include "kroki/ring_buffer.h"
#include <kroki/error.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <limits.h>
#include <errno.h>


static struct option options[] = {
  { .name = "read", .val = 'r' },
  { .name = "write", .val = 'w' },
  { .name = "non-block", .val = 'n' },
  { .name = "size", .val = 's', .has_arg = required_argument },
  { .name = "version", .val = 'v' },
  { .name = "help", .val = 'h' },
  { .name = NULL },
};


static
void
usage(FILE *out)
{
  fprintf(out,
          "Usage: %s [OPTIONS] RINGBUFFERFILE\n"
          "\n"
          "Options are:\n"
          "  --read, -r                  Read data from file\n"
          "  --write, -w                 Write data to file\n"
          "  --non-block, -n             Use non-blocking operations\n"
          "  --size, -s NUM              Buffer size in bytes (0-4294967295)\n"
          "  --version, -v               Print package version and copyright\n"
          "  --help, -h                  Print this message\n",
          program_invocation_short_name);
}


static
void
version(FILE *out)
{
  fprintf(out,
          "%s\n"
          "%s\n"
          "Report bugs to <%s> or file an issue at\n"
          "<%s>.\n",
          PACKAGE_STRING,
          PACKAGE_COPYRIGHT,
          PACKAGE_BUGREPORT,
          PACKAGE_URL);
}


static const char *rb_filename;
static bool rb_read;
static bool rb_write;
static bool rb_non_block;
static unsigned int rb_size;


static
void
process_args(int argc, char *argv[])
{
  int opt;
  while ((opt = getopt_long(argc, argv, "rwns:vh", options, NULL)) != -1)
    {
      switch (opt)
        {
        case 'r':
          rb_read = true;
          break;

        case 'w':
          rb_write = true;
          break;

        case 'n':
          rb_non_block = true;
          break;

        case 's':
          {
            char *end;
            errno = 0;
            unsigned long size = strtoul(optarg, &end, 0);
            if (*end || size > 0xffffffff || (size == ULONG_MAX && errno))
              {
                error("size %s is not valid (should be in range 0-4294967295)",
                      optarg);
              }
            rb_size = size;
          }
          break;

        case 'v':
          version(stdout);
          exit(EXIT_SUCCESS);

        case 'h':
          usage(stdout);
          exit(EXIT_SUCCESS);

        default:
          usage(stderr);
          exit(EXIT_FAILURE);
        }
    }
  if (optind != argc - 1)
    {
      usage(stderr);
      exit(EXIT_FAILURE);
    }
  if (rb_read == rb_write)
    {
      warn("exactly one of --read or --write must be given");
      usage(stderr);
      exit(EXIT_FAILURE);
    }

  rb_filename = argv[optind++];
}


int
main(int argc, char *argv[])
{
  process_args(argc, argv);

  struct ring_buffer *rb = ring_buffer_open(rb_filename, rb_size);
  if (! rb)
    error("opening %s: %m", rb_filename);

  if (rb_write)
    {
      while (1)
        {
          unsigned int size = (rb_non_block ? 0 : 1);
          char *buf = ring_buffer_get_free(rb, &size);
          if (! size)
            break;
          size = fread(buf, 1, size, stdin);
          if (! size)
            break;
          ring_buffer_add(rb, size);
        }
    }
  else
    {
      while (1)
        {
          unsigned int size = (rb_non_block ? 0 : 1);
          char *data = ring_buffer_get_data(rb, &size);
          if (! size)
            break;
          CHECK(fwrite(data, size, 1, stdout), != 1, die, "%m");
          ring_buffer_del(rb, size);
        }
    }

  ring_buffer_close(rb);

  return EXIT_SUCCESS;
}
