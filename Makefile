CC ?= cc
AR ?= ar
XFLAGS = -fPIC -larchive

SOURCES = src/archive.c src/database.c src/utils.c

OBJECTS = src/archive.o src/database.o src/utils.o

LIBRARY = lib/libusagi.so.1

LIBRARY_STATIC = lib/libusagi.a

all: dirs shared static

dirs:
	mkdir -p lib

static: $(OBJECTS)
	$(AR) rcs $(LIBRARY_STATIC) $(OBJECTS)

shared: $(OBJECTS)
	$(CC) $(XFLAGS) $(CFLAGS) $(LDFLAGS) -shared $(OBJECTS) -o $(LIBRARY)

src/archive.o: src/archive.c
	$(CC) $(CFLAGS) -c src/archive.c -o src/archive.o

src/database.o: src/database.c
	$(CC) $(CFLAGS) -c src/database.c -o src/database.o

src/utils.o: src/utils.c
	$(CC) $(CFLAGS) -c src/utils.c -o src/utils.o

clean:
	rm $(LIBRARY) $(LIBRARY_STATIC) $(OBJECTS) || true
	rmdir lib 
