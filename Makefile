CC = gcc
CCFLAGS = -fPIC
LDFLAGS = -lzstd

SOURCES = src/archive.c src/database.c src/utils.c

OBJECTS = src/archive.o src/database.o src/utils.o

LIBRARY = lib/libusagi.so

all: dirs $(LIBRARY)

dirs:
	mkdir lib || true

$(LIBRARY): $(OBJECTS)
	$(CC) $(CCFLAGS) $(LDFLAGS) -shared $(OBJECTS) -o $(LIBRARY)

src/archive.o: src/archive.c
	$(CC) $(CCFLAGS) -c src/archive.c -o src/archive.o

src/database.o: src/database.c
	$(CC) $(CCFLAGS) -c src/database.c -o src/database.o

src/utils.o: src/utils.c
	$(CC) $(CCFLAGS) -c src/utils.c -o src/utils.o

clean:
	rm $(LIBRARY) $(OBJECTS) || true
	rmdir lib || true
