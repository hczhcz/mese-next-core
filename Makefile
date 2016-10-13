CFLAGS = -std=c++14 -Wall -pedantic
CFLAGS_DEBUG = -std=c++14 -Wall -pedantic -g -O0
HEADERS = $(wildcard *.hpp)
FILES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(FILES))

default: mese

mese: $(FILES) $(HEADERS)
	clang++ $(CFLAGS) $(FILES) -o $@

mese32: $(FILES) $(HEADERS)
	clang++ -m32 $(CFLAGS) $(FILES) -o $@

mese-gcc: $(FILES) $(HEADERS)
	g++ $(CFLAGS) $(FILES) -o $@

mese32-gcc: $(FILES) $(HEADERS)
	g++ -m32 $(CFLAGS) $(FILES) -o $@

mese.exe: $(FILES) $(HEADERS)
	x86_64-w64-mingw32-g++ -static-libstdc++ -static-libgcc $(CFLAGS) $(FILES) -o $@

mese32.exe: $(FILES) $(HEADERS)
	i686-w64-mingw32-g++ -static-libstdc++ -static-libgcc $(CFLAGS) $(FILES) -o $@

%.o: %.cpp $(HEADERS)
	clang++ -c $(CFLAGS_DEBUG) $< -o $@

debug: $(OBJECTS)
	clang++ $(CFLAGS_DEBUG) $(OBJECTS) -o mese

all: mese mese32 mese-gcc mese32-gcc mese.exe mese32.exe

clean:
	rm -f mese mese32 mese-gcc mese32-gcc mese.exe mese32.exe $(OBJECTS)
