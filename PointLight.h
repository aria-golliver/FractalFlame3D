#ifndef __POINTLIGHT
#define __POINTLIGHT

#include "GenVector.h"
#include <cmath>

class PointLight {
public:
    PointLight(Vector3 pos, Vector3 amb, Vector3 diff, Vector3 spec): 
        pos(pos),
        amb(amb),
        diff(diff),
        spec(spec)
    {}

	const Vector3 pos, amb, diff, spec;

    void print() const {
        pos.print();
        amb.print();
    }
};

#endif