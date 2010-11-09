MAIGRE_SOURCES =  \
	Cairo/ColorExtensions.cs \
	Cairo/ContextExtensions.cs \
	Cairo/Corner.cs \
	Cairo/RectangleExtensions.cs \
	Maigre.Osx/OsxTheme.cs \
	Maigre/Theme.cs \
	Maigre/Theme_Generated.cs

MAIGRE_OTHER = gtkrc

REFERENCES =  \
	Mono.Cairo \
	-pkg:gtk-sharp-2.0 \
	System \
	System.Core

REFERENCES_BUILD = \
	$(filter -pkg:%, $(REFERENCES)) \
	$(foreach r, $(filter-out -pkg:%, $(REFERENCES)), -r:$(r))

all: Maigre.dll

Maigre.dll: $(MAIGRE_SOURCES)
	gmcs -out:$@ -target:library -debug $(REFERENCES_BUILD) $(MAIGRE_SOURCES)

Maigre/Theme_Generated.cs: ../maigre-codegen/maigre-codegen.py
	$(MAKE) -C ../maigre-codegen

run:
	$(MAKE) -C .. run

clean:
	rm -rf Maigre.dll* obj/ Maigre/Theme_Generated.cs
