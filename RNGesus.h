#ifndef __RNGESUS
#define __RNGESUS

#include <cstdint>
#include <cstdlib>
#include <climits>
#include <time.h>

class RNGesus {
public:
    RNGesus(){
        seed = 0;
    }

    void kneel(){
        seed ^= (uint64_t)rand() << (uint64_t)32 | (uint64_t)rand();
    }

    float pray(){
	    seed ^= (seed << 21);
	    seed ^= (seed >> 35);
	    seed ^= (seed << 4);
	    return (float) ((int64_t)seed) / (INT64_MAX);
    }

private:
    uint64_t seed;
};

#endif