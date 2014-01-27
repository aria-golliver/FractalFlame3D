CC=colorgcc
CFLAGS=      -Wall -O3 -std=c++11 -fopenmp -march=native -ffast-math
CFLAGSDEBUG= -Wall -O0 -std=c++11 -fopenmp -march=native -g

CFLAGSVEC=      -Wall -O3 -std=c++11 -fopenmp -march=native -ffast-math -ftree-vectorizer-verbose=1

all: release

debug:
	$(CC) $(CFLAGSDEBUG) *.cpp -o ray

release:
	$(CC) $(CFLAGS) *.cpp -o ray
vec:
	$(CC) $(CFLAGSVEC) *.cpp -o ray
