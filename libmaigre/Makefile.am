EXTRA_DIST = maigre-style.c.in
CLEANFILES = maigre-style.c
MAINTAINERCLEANFILES = Makefile.in

INCLUDES = \
	-Wall -ggdb3 -D_FORTIFY_SOURCE=2 \
	$(MONO_CFLAGS) \
	$(GTK_CFLAGS)

MAIGRE_SRC = \
	maigre-mono-bridge.c \
	maigre-mono-bridge.h \
	maigre-rc-style.c \
	maigre-rc-style.h \
	maigre-style.h \
	maigre-theme-module.c

enginedir = $(libdir)/gtk-2.0/$(GTK_VERSION)/engines
engine_LTLIBRARIES = libmaigre.la

libmaigre_la_SOURCES = $(MAIGRE_SRC) maigre-style.c
libmurrine_la_LDFLAGS = -module -avoid-version -no-undefined
libmurrine_la_LIBADD = $(MONO_LIBS) $(GTK_LIBS)

maigre-style.c: maigre-style.c.in
	@echo "  GEN    $@"; ../maigre-codegen/maigre-codegen.py $< $@
