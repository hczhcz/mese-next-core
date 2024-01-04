CFLAGS = -std=c++14 -Wall -pedantic -O2
CFLAGS_DEBUG = -std=c++14 -Wall -pedantic -g -O0
CFLAGS_PUBLIC = -std=c++14 -Wall -pedantic -O2
HEADERS = $(wildcard *.hpp)
FILES = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(FILES))

default: mese

mese: $(FILES) $(HEADERS)
	clang++ -stdlib=libc++ $(CFLAGS) $(FILES) -o $@

mese32: $(FILES) $(HEADERS)
	clang++ -m32 -stdlib=libc++ $(CFLAGS) $(FILES) -o $@

mese-gcc: $(FILES) $(HEADERS)
	g++ $(CFLAGS) $(FILES) -o $@

mese32-gcc: $(FILES) $(HEADERS)
	g++ -m32 $(CFLAGS) $(FILES) -o $@

mese.exe: $(FILES) $(HEADERS)
	x86_64-w64-mingw32-g++ -static-libstdc++ -static-libgcc $(CFLAGS) $(FILES) -o $@

mese32.exe: $(FILES) $(HEADERS)
	i686-w64-mingw32-g++ -static-libstdc++ -static-libgcc $(CFLAGS) $(FILES) -o $@

%.o: %.cpp $(HEADERS)
	clang++ -c -stdlib=libc++ $(CFLAGS_DEBUG) $< -o $@

mese-debug: $(OBJECTS)
	clang++ -stdlib=libc++ $(CFLAGS_DEBUG) $(OBJECTS) -o $@

mese-public: $(FILES) $(HEADERS)
	clang++ -stdlib=libc++ $(CFLAGS_PUBLIC) $(FILES) -o $@

all: mese mese-gcc mese.exe mese-debug mese-public

all32: mese32 mese32-gcc mese32.exe

clean:
	rm -f mese mese32 mese-gcc mese32-gcc mese.exe mese32.exe mese-debug mese-public $(OBJECTS)
