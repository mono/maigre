MAIGRE_SOURCES = \
	Cairo/ColorExtensions.cs \
	Cairo/ContextExtensions.cs \
	Cairo/Corner.cs \
	Cairo/RectangleExtensions.cs \
	Maigre.Osx/OsxTheme.cs

MAIGRE_TEMPLATE_SOURCES_IN = \
	Maigre/Theme_Template.cs

MAIGRE_TEMPLATE_SOURCES_OUT = \
	Maigre/Theme.cs

Maigre.dll: $(MAIGRE_SOURCES) $(MAIGRE_TEMPLATE_SOURCES_OUT)
	@echo "  GMCS   $@"; $(MCS) -out:$@ -target:library -debug -r:Mono.Cairo -r:System.Core $(GTK_SHARP_LIBS) $(MAIGRE_SOURCES) $(MAIGRE_TEMPLATE_SOURCES_OUT)

Maigre/Theme.cs: Maigre/Theme_Template.cs
	@echo "  GEN    $@"; ../maigre-codegen/maigre-codegen.py $< $@

all: Maigre.dll

EXTRA_DIST = gtkrc
CLEANFILES = Maigre.dll{,.mdb} $(MAIGRE_TEMPLATE_SOURCES_OUT)
MAINTAINERCLEANFILES = Makefile.in Maigre.pidb
