all:
	$(MAKE) -C maigre-codegen
	$(MAKE) -C libmaigre
	$(MAKE) -C Maigre

clean:
	$(MAKE) -C maigre-codegen clean
	$(MAKE) -C libmaigre clean
	$(MAKE) -C Maigre clean
	rm -rf lib

run:
	rm -rf lib
	mkdir -p lib/engines
	cp Maigre/gtkrc lib
	cp libmaigre/libmaigre.so Maigre/Maigre.dll lib/engines
	GTK_PATH=$$PWD/lib GTK2_RC_FILES=lib/gtkrc gtk-demo
