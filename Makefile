CFLAGS = -std=c++14 -Wall -pedantic
FILES = $(wildcard *.cpp)

default: mese

mese: $(FILES)
	clang++ $(CFLAGS) $(FILES) -o $@

mese32: $(FILES)
	clang++ -m32 $(CFLAGS) $(FILES) -o $@

mese-gcc: $(FILES)
	g++ $(CFLAGS) $(FILES) -o $@

mese32-gcc: $(FILES)
	g++ -m32 $(CFLAGS) $(FILES) -o $@

mese.exe: $(FILES)
	x86_64-w64-mingw32-g++ -static-libstdc++ -static-libgcc $(CFLAGS) $(FILES) -o $@

mese32.exe: $(FILES)
	i686-w64-mingw32-g++ -static-libstdc++ -static-libgcc $(CFLAGS) $(FILES) -o $@

all: mese mese32 mese-gcc mese32-gcc mese.exe mese32.exe

clean:
	rm -f mese mese32 mese-gcc mese32-gcc mese.exe mese32.exe
