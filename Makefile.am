## Process this file with automake to produce Makefile.in.
##

include $(top_srcdir)/Makefile.common


ACLOCAL_AMFLAGS = -I m4


SUBDIRS =					\
	src/


EXTRA_DIST =					\
	version.sh				\
	kroki-ring-buffer.spec


rpm: dist
	HOME=$(abs_top_builddir) rpmbuild -tb $(distdir).tar.bz2
	mv $(top_builddir)/rpmbuild/RPMS/*/*.rpm $(top_builddir)
	rm -rf $(top_builddir)/rpmbuild
	rm $(distdir).tar.bz2
