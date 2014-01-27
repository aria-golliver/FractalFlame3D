#ifndef __AABBOX
#define __AABBOX

#include "GenVector.h"
#include "Ray.h"
#include <cmath>
#include "Surface.h"

class AABBox {
public:
	AABBox(){
		this->min_point = Vector3();
		this->max_point = Vector3();
	}

	AABBox(Vector3 min_point, Vector3 max_point){
		this->min_point = min_point;
		this->max_point = max_point;
	}

    
	// returns -1 if the ray and the object do not intersect
	//
	// returns 0...+inf as the distance between the ray's origin
	// and where on the object intersection occurs
    // this one is a little bit faster that hit because it only computes t
    inline virtual float hit_test(const Ray& ray) const {
        float tx0 = (min_point[0] - ray.pos[0]) * ray.inv[0];
        float tx1 = (max_point[0] - ray.pos[0]) * ray.inv[0];

        float ty0 = (min_point[1] - ray.pos[1]) * ray.inv[1];
        float ty1 = (max_point[1] - ray.pos[1]) * ray.inv[1];

        float tz0 = (min_point[2] - ray.pos[2]) * ray.inv[2];
        float tz1 = (max_point[2] - ray.pos[2]) * ray.inv[2];

        float tmin = min2(tx0, tx1);
        float tmax = max2(tx0, tx1);
        
        tmin = max2(tmin, min2(ty0, ty1));
        tmax = min2(tmax, max2(ty0, ty1));

        tmin = max2(tmin, min2(tz0, tz1));
        tmax = min2(tmax, max2(tz0, tz1));

        float t = -1;

        if(tmax >= 0)
            t = tmax;

        if(tmin >= 0)
            t = tmin;


		return tmax >= tmin ? t : -1.0f;
	}

	Vector3 min_point, max_point;
    
private:

	inline float square(const float f) const {
		return f*f;
	}
};

#endif