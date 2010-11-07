LIBMAIGRE_SOURCES =  \
	maigre-mono-bridge.c \
	maigre-rc-style.c \
	maigre-style.c \
	maigre-theme-module.c

LIBMAIGRE_OTHER =  \
	maigre-gtk-style-generator \
	maigre-mono-bridge.h \
	maigre-rc-style.h \
	maigre-style.h

PC_DEPS = \
	mono \
	gtk+-2.0

CC = gcc

LIBMAIGRE_CFLAGS = \
	-Wall -O2 -ggdb3 \
	$(shell pkg-config --cflags $(PC_DEPS)) \
	$(CFLAGS)

LIBMAIGRE_LDFLAGS = \
	-shared \
	$(shell pkg-config --libs $(PC_DEPS)) \
	$(LDFLAGS)

LIBMAIGRE_OBJECTS = $(LIBMAIGRE_SOURCES:.c=.o)

all: libmaigre.so

libmaigre.so: maigre-style-overrides.c $(LIBMAIGRE_OBJECTS)
	$(CC) $(LIBMAIGRE_LDFLAGS) $(LIBMAIGRE_OBJECTS) -o $@

.c.o:
	$(CC) -c $(LIBMAIGRE_CFLAGS) $< -o $@

maigre-style-overrides.c: maigre-gtk-style-generator
	./$<

clean:
	rm -rf $(LIBMAIGRE_OBJECTS) libmaigre.so maigre-style-overrides.c

test:
	$(MAKE) -C .. test
