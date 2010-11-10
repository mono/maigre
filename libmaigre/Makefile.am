INCLUDES = \
	-Wall -ggdb3 -D_FORTIFY_SOURCE=2 \
	$(MONO_CFLAGS) \
	$(GTK_CFLAGS)

MAIGRE_SRC =  \
	maigre-mono-bridge.c \
	maigre-theme-module.c

MAIGRE_OTHER =  \
	maigre-mono-bridge.h \
	maigre-rc-style.h \
	maigre-style.h \
	maigre-rc-style.template.c \
	maigre-style.template.c

enginedir = $(libdir)/gtk-2.0/$(GTK_VERSION)/engines
engine_LTLIBRARIES = libmaigre.la

libmaigre_la_SOURCES = $(MAIGRE_SRC)
nodist_libmaigre_la_SOURCES = maigre-style.c maigre-rc-style.c
libmaigre_la_LDFLAGS = -module -avoid-version -no-undefined
libmaigre_la_LIBADD = $(MONO_LIBS) $(GTK_LIBS)

maigre-style.c: maigre-style.template.c $(top_srcdir)/maigre-codegen/maigre-codegen.py
	@echo "  GEN    $@"; $(top_srcdir)/maigre-codegen/maigre-codegen.py $< $@

maigre-rc-style.c: maigre-rc-style.template.c $(top_srcdir)/maigre-codegen/maigre-codegen.py
	@echo "  GEN    $@"; $(top_srcdir)/maigre-codegen/maigre-codegen.py $< $@

EXTRA_DIST = $(MAIGRE_OTHER) libmaigre.cproj
CLEANFILES = maigre-style.c maigre-rc-style.c
MAINTAINERCLEANFILES = Makefile.in libmaigre.pidb libmaigre.md.pc
