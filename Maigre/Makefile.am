MAIGRE_SOURCES =  \
	Cairo/ColorExtensions.cs \
	Cairo/ContextExtensions.cs \
	Cairo/Corner.cs \
	Cairo/RectangleExtensions.cs \
	Maigre.Osx/OsxRcStyle.cs \
	Maigre.Osx/OsxStyle.cs \
	Maigre/ThemeModule.cs

MAIGRE_TEMPLATE_SOURCES_IN =  \
	Maigre/RcStyle_Template.cs \
	Maigre/Style_Template.cs

MAIGRE_TEMPLATE_SOURCES_OUT = \
	MaigreRcStyle.cs \
	MaigreStyle.cs

MAIGRE_SOURCES_BUILD = $(foreach src, $(MAIGRE_SOURCES), $(srcdir)/$(src)) $(MAIGRE_TEMPLATE_SOURCES_OUT)

Maigre.dll: $(MAIGRE_SOURCES_BUILD)
	@echo "  GMCS   $@"; $(MCS) -out:$@ -target:library -debug -r:Mono.Cairo -r:System.Core $(GTK_SHARP_LIBS) $(MAIGRE_SOURCES_BUILD)

MaigreRcStyle.cs: Maigre/RcStyle_Template.cs $(top_srcdir)/maigre-codegen/maigre-codegen.py
	@echo "  GEN    $@"; $(top_srcdir)/maigre-codegen/maigre-codegen.py $< $@

MaigreStyle.cs: Maigre/Style_Template.cs $(top_srcdir)/maigre-codegen/maigre-codegen.py
	@echo "  GEN    $@"; $(top_srcdir)/maigre-codegen/maigre-codegen.py $< $@

all: Maigre.dll

EXTRA_DIST = gtkrc Maigre.csproj $(MAIGRE_TEMPLATE_SOURCES_IN) $(MAIGRE_SOURCES)
CLEANFILES = Maigre.dll{,.mdb} $(MAIGRE_TEMPLATE_SOURCES_OUT)
MAINTAINERCLEANFILES = Makefile.in Maigre.pidb
