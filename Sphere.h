#ifndef __SPHERE
#define __SPHERE

#include "Surface.h"
#include "GenVector.h"
#include "Ray.h"
#include <cmath>

class Sphere : public Surface {
public:
	Sphere(Vector3 pos, float rad, Vector3 mat_ambient){
		this->pos = pos;
		this->radius = rad;

		
        this->mat_ambient = mat_ambient;
		/*
		this->mat_diffuse = mat_diffuse;
        this->mat_specular = mat_specular;
        this->shiny = shiny;
        this->reflect = reflect;
		this->alpha = alpha;
		*/
		this->min_point = pos - rad;
		this->max_point = pos + rad;
	}

    
	// returns -1 if the ray and the object do not intersect
	//
	// returns 0...+inf as the distance between the ray's origin
	// and where on the object intersection occurs
    // this one is a little bit faster that hit because it only computes t
    virtual float hit_test(const Ray& ray) const {
        
		const Vector3 d = ray.dir;
		const Vector3 e = ray.pos;
		const Vector3 c = this->pos;
		const float R = this->radius;

		const float A = d.dot(d);
		const float B = 2.0f * d.dot(e-c);
		const float C = ((e-c).dot(e-c)) - (R*R);
		const float discriminant = (B*B) - 4.0f*A*C;

		if(discriminant < 0.0f)
            return -1;
        
		const float t1 = (-B - sqrt(discriminant)) / (2.0f*A);
		const float t2 = (-B + sqrt(discriminant)) / (2.0f*A);
        const float t = min2(t1, t2);

        return (t > epsilon) ? t : -1;

	}
    
    virtual HitVector hit(const Ray& ray) const {
        HitVector hv;

        float t = hit_test(ray);
        
        if(t < 0){
            hv.hit = false;
            return hv;
        }

        const Vector3 hitPoint = (ray.dir * t) + ray.pos;
        const Vector3 normal = normalAt(hitPoint);
        
        hv.hit = true;
		hv.t = t;
        hv.hitPoint = hitPoint;
        hv.normal = normal;
        hv.ray = ray;

        return hv;
    }
    
    
    Vector3 shade(const Vector3 p) const {
        return normalAt(p);
    };
    

    Vector3 normalAt(const Vector3 p) const {
        return (p - pos).normalize();
    };
private:
	Vector3 pos;
	float radius;

	float square(const float f) const {
		return f*f;
	}
};

#endif