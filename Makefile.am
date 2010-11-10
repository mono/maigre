SUBDIRS = \
	maigre-codegen \
	libmaigre \
	Maigre

EXTRA_DIST = \
	Maigre.sln

MAINTAINERCLEANFILES = \
	Makefile.in \
	INSTALL \
	aclocal.m4 \
	config.guess \
	config.sub \
	configure \
	depcomp \
	install-sh \
	ltmain.sh \
	missing \
	mkinstalldirs

.PHONY: run

RUNDIR=$(top_builddir)/.run

run:
	rm -rf $(RUNDIR)
	mkdir -p $(RUNDIR)/engines
	cp -a libmaigre/.libs/libmaigre.so Maigre/Maigre.dll{,.mdb} $(RUNDIR)/engines
	cp Maigre/gtkrc $(RUNDIR)
	GTK_PATH=$(RUNDIR) GTK2_RC_FILES=$(RUNDIR)/gtkrc $$(which twf 2>/dev/null || gtk-demo)
