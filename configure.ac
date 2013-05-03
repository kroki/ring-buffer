dnl Process this file with autoconf to produce a configure script.
dnl
dnl To bootstrap run './bootstrap.sh'.
dnl

m4_define([kroki_ring_buffer_copyright],
          [patsubst([
Copyright (C) 2013 Tomash Brechko.  All rights reserved.

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
], [
], [\\n])])

AC_PREREQ(2.66)
m4_include([version.m4])
AC_INIT([Kroki/ring-buffer], [VERSION_STRING], [tomash.brechko@gmail.com],
        [kroki-ring-buffer], [https://github.com/kroki/ring-buffer])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_AUX_DIR([auto])
AC_CONFIG_MACRO_DIR([m4])

AC_USE_SYSTEM_EXTENSIONS

AM_INIT_AUTOMAKE([1.11.1 -Wall foreign no-define subdir-objects
                  dist-bzip2 no-dist-gzip color-tests])

AC_PREFIX_DEFAULT([/usr/local])

AC_CONFIG_SRCDIR([Makefile.common])

AM_PROG_AR

LT_INIT([disable-static])

AC_PROG_MAKE_SET
AC_PROG_CC
AC_PROG_CC_C99

AC_CHECK_HEADER([kroki/error.h], [],
                [AC_MSG_ERROR([kroki/error.h is required])])

AC_SUBST([RPM_VERSION], [`echo VERSION_STRING | sed -e 's/-.*//'`])
AC_SUBST([RPM_RELEASE], [`echo VERSION_STRING | sed -e 's/[[^-]]*-\?//'`])
AC_DEFINE([PACKAGE_COPYRIGHT], ["kroki_ring_buffer_copyright"],
          [Copyright string.])
AC_SUBST_FILE([README])
README=$srcdir/README

AC_CACHE_SAVE

AC_CONFIG_FILES([
kroki-ring-buffer.spec
Makefile
src/Makefile
])
AC_CONFIG_FILES([stamp-h], [echo timestamp > stamp-h])

AC_OUTPUT
