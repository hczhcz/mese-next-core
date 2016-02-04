CFLAGS = -std=c++14 -Wall -pedantic
FILES = $(wildcard *.cpp)

default: mese

mese: $(FILES) mese.hpp
	clang++ $(CFLAGS) $(FILES) -o $@

mese32: $(FILES) mese.hpp
	clang++ -m32 $(CFLAGS) $(FILES) -o $@

mese-gcc: $(FILES) mese.hpp
	g++ $(CFLAGS) $(FILES) -o $@

mese32-gcc: $(FILES) mese.hpp
	g++ -m32 $(CFLAGS) $(FILES) -o $@

mese.exe: $(FILES) mese.hpp
	x86_64-w64-mingw32-g++ -static-libstdc++ -static-libgcc $(CFLAGS) $(FILES) -o $@

mese32.exe: $(FILES) mese.hpp
	i686-w64-mingw32-g++ -static-libstdc++ -static-libgcc $(CFLAGS) $(FILES) -o $@

all: mese mese32 mese-gcc mese32-gcc mese.exe mese32.exe

clean:
	rm -f mese mese32 mese-gcc mese32-gcc mese.exe mese32.exe
