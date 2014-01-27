#ifndef __RDRAND_H__
#define __RDRAND_H__ __RDRAND_H__

#include <cstdint>
#include <cstdlib>
#include <immintrin.h>

#define RDRAND_SUCCESS 1

//#define FASTRAND

#ifdef FASTRAND

inline void rdrand_u16(uint16_t *p){
    while( _rdrand16_step(p) != RDRAND_SUCCESS);
}

inline void rdrand_u32(uint32_t *p){
    while( _rdrand32_step(p) != RDRAND_SUCCESS);
}

inline void rdrand_u64(uint64_t *p){
    while( _rdrand64_step((unsigned long long *) p) != RDRAND_SUCCESS);
}

inline void rdrand_i16(int16_t *p){
    rdrand_u16((uint16_t *)p);
}

inline void rdrand_i32(int32_t *p){
    rdrand_u32((uint32_t *)p);
}

inline void rdrand_i64(int64_t *p){
    rdrand_u64((uint64_t *)p);
}

// floating point random functions return (-1, 1)
inline void rdrand_f32(float *p){
    int32_t tmp;
    rdrand_i32(&tmp);
    *p = tmp / (float)INT32_MAX;
}

// floating point random functions return (-1, 1)
inline void rdrand_f64(double *p){
    int64_t tmp;
    rdrand_i64(&tmp);
    *p = tmp / (double)INT64_MAX;
}

#else
inline void rdrand_f32(float *p){
	*p = (float)rand() / RAND_MAX;
}
inline void rdrand_u32(unsigned int *p){
	*p = (unsigned int)rand();
}

#endif

#endif