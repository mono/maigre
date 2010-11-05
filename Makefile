clean:
	rm -rf engines
	rm -rf Maigre.dll*

test:
	cp engines/Maigre.dll* .
	GTK_PATH=$$PWD GTK2_RC_FILES=maigre.gtkrc gtk-demo
