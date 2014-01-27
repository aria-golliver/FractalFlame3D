#ifndef __SURFACELIST
#define __SURFACELIST

#include "Surface.h"
#include "HitVector.h"
#include "PointLight.h"
#include <cstdlib>
#include <cmath>
#include <vector>

using namespace std;

class SurfaceList {
public:
    const vector<const Surface *> *surfaces;
	AABBox AABB;

	SurfaceList(){}

    SurfaceList(const vector<const Surface *> *surfaces){
        this->surfaces = surfaces;

		float min_x =  std::numeric_limits<float>::infinity();
		float min_y =  std::numeric_limits<float>::infinity();
		float min_z =  std::numeric_limits<float>::infinity();

		float max_x = -std::numeric_limits<float>::infinity();
		float max_y = -std::numeric_limits<float>::infinity();
		float max_z = -std::numeric_limits<float>::infinity();

		for (const Surface *s : *surfaces){
			min_x = min2(min_x, s->min_point[0]);
			min_y = min2(min_y, s->min_point[1]);
			min_z = min2(min_z, s->min_point[2]);

			max_x = max2(max_x, s->max_point[0]);
			max_y = max2(max_y, s->max_point[1]);
			max_z = max2(max_z, s->max_point[2]);
		}

		AABB = AABBox(Vector3(min_x, min_y, min_x), Vector3(max_x, max_y, max_z));
    }


    virtual const  Surface * intersectScene(const Ray &ray, HitVector &hit) const {
		const Surface *nearestSurface = NULL;
        float t = std::numeric_limits<float>::infinity();
        
        for(const Surface *surface : *surfaces){
            HitVector hv = surface->hit(ray);
		
            if(hv.hit && hv.t < t){
                hit = hv;
                t = hv.t;
                nearestSurface = surface;
            }
		}

        return nearestSurface;
    }

    /*
     * A much simpler version of intersectScene
     * this function just checkes to see if any objects are closer than [max] distance away from the ray
     * very useful for shadow checking
     */
    virtual bool intersectScene_max(const Ray &ray, float max) const {
        for(const Surface *surface : *surfaces){
            float t = surface->hit_test(ray);
            if( t >= 0 && t <= max ){
                return true;
            }
        }

        return false;
    }

	virtual int size() const {
		return surfaces->size();
	}

	virtual void print(){
		printf("flat surface list with %d items\n", size());
	}
};

#endif
