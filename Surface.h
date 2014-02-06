#ifndef __SURFACE
#define __SURFACE

#include "Ray.h"
#include "AABBox.h"
#include "HitVector.h"
#include "PointLight.h"
#include <vector>


class Surface {
public:
	// returns -1 if the ray and the object do not intersect
	//
	// returns 0...+inf as the distance between the ray's origin
	// and where on the object intersection occurs
    virtual HitVector hit(const Ray& ray) const {
        HitVector hv;
        hv.hit = false;
        
        return hv;
    };

    virtual float hit_test(const Ray& ray) const {
        return false;
    };
    
    //Vector3 mat_ambient, mat_diffuse, mat_specular;
    //float shiny, reflect, alpha;

	Vector3 min_point, max_point;
	Vector4 mat_ambient;
	//AABBox AABB;
};

#endif
