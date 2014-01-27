#ifndef __TRIANGLE
#define __TRIANGLE

#include "Surface.h"
#include "GenVector.h"
#include "Ray.h"
#include <cmath>

class Triangle : public Surface {
public:
	Triangle(Vector3 vert1, Vector3 vert2, Vector3 vert3)
    {
        setVerts(vert1, vert2, vert3);
        //this->mat_ambient = this->mat_diffuse = this->mat_specular = Vector3(0, 0, 0);
	}
    
	Triangle(Vector3 vert1, Vector3 vert2, Vector3 vert3, const Vector3 mat_ambient, const Vector3 mat_diffuse, const Vector3 mat_specular, float shiny, float reflect)
    {
        setVerts(vert1, vert2, vert3);
        
		/*
        this->mat_ambient = mat_ambient;
        this->mat_diffuse = mat_diffuse;
        this->mat_specular = mat_specular;
        this->shiny = shiny;
        this->reflect = reflect;
		*/
	}

    void setVerts(Vector3 vert1, Vector3 vert2, Vector3 vert3){
		this->a = vert1;
		this->b = vert2;
		this->c = vert3;
        n = ((a - b).cross(b - c)).normalize();

		for (int i = 0; i < 3; i++){
			this->min_point[i] = min3(vert1[i], vert2[i], vert3[i]);
			this->max_point[i] = max3(vert1[i], vert2[i], vert3[i]);
		}
    }
    
    virtual HitVector hit(const Ray& ray) const {
        HitVector hv;
        
        float t = hit_test(ray);
        if(t < 0) {
            hv.hit = false;
            return hv;
        }

        // this is computed twice, but whatever
        Vector3 hitPoint = ray.pos + ray.dir * t;

        hv.hit = true;
		hv.t = t;
        hv.hitPoint = hitPoint;
        hv.normal = n;
        hv.ray = ray;

        return hv;
	}

	// returns -1 if the ray and the object do not intersect
	//
	// returns 0...+inf as the distance between the ray's origin
	// and where on the object intersection occurs
    // this one is a little bit faster that hit because it only computes t
    virtual float hit_test(const Ray& ray) const {
        
        float t = (a - ray.pos).dot(n) / (ray.dir.dot(n));

        Vector3 x = ray.pos + ray.dir * t;
        
        if(t >= 0)
        if((b-a).cross(x-a).dot(n) > 0)
        if((c-b).cross(x-b).dot(n) > 0)
        if((a-c).cross(x-c).dot(n) > 0)
            return t;
        return -1;
	}
    
    Vector3 normalAt() const {
        return n;
    };

	Vector3 a;
    Vector3 b;
    Vector3 c;

    Vector3 n;
};

#endif