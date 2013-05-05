## Process this file with automake to produce Makefile.in.
##

include $(top_srcdir)/Makefile.common


bin_PROGRAMS =					\
	kroki-ring-buffer


kroki_ring_buffer_LDADD =			\
	libkroki-ring-buffer.la


nobase_include_HEADERS =			\
	kroki/ring_buffer.h			\
	kroki/bits/ring_buffer.h


lib_LTLIBRARIES =				\
	libkroki-ring-buffer.la


## See 'info libtool versioning updating' for how to update version number.
libkroki_ring_buffer_la_LDFLAGS =		\
	-version-info 0:0:0
