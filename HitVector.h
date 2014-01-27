#ifndef __HITVECTOR
#define __HITVECTOR

#include "GenVector.h"
#include "Surface.h"
#include "Ray.h"

typedef struct {
    bool hit;
    Vector3 normal;
    Vector3 hitPoint;
    Vector3 totalLight;
    Ray ray;
    float t;
} HitVector;

#endif