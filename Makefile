CFLAGS = -std=c++14 -Wall -pedantic
HEADERS = $(wildcard *.hpp)
FILES = $(wildcard *.cpp)

default: mese

mese: $(HEADERS) $(FILES)
	clang++ $(CFLAGS) $(FILES) -o $@

mese32: $(HEADERS) $(FILES)
	clang++ -m32 $(CFLAGS) $(FILES) -o $@

mese-gcc: $(HEADERS) $(FILES)
	g++ $(CFLAGS) $(FILES) -o $@

mese32-gcc: $(HEADERS) $(FILES)
	g++ -m32 $(CFLAGS) $(FILES) -o $@

mese.exe: $(HEADERS) $(FILES)
	x86_64-w64-mingw32-g++ -static-libstdc++ -static-libgcc $(CFLAGS) $(FILES) -o $@

mese32.exe: $(HEADERS) $(FILES)
	i686-w64-mingw32-g++ -static-libstdc++ -static-libgcc $(CFLAGS) $(FILES) -o $@

all: mese mese32 mese-gcc mese32-gcc mese.exe mese32.exe

clean:
	rm -f mese mese32 mese-gcc mese32-gcc mese.exe mese32.exe
