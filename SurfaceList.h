#ifndef __SURFACELIST
#define __SURFACELIST

#include "Surface.h"
#include "HitVector.h"
#include "PointLight.h"
#include <cstdlib>
#include <cmath>
#include <vector>

using namespace std;

class SurfaceList : public Surface {
public:
    const vector<Surface *> *surfaces;
    Surface LHS, RHS;

    SurfaceList(const vector<Surface *> *surfaces){
        this->surfaces = surfaces;
    }


    Surface * intersectScene(const Ray &ray, HitVector &hit) const {
		Surface *nearestSurface = NULL;
        float t = std::numeric_limits<float>::infinity();
        
        for(Surface *surface : *surfaces){
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
    bool intersectScene_max(const Ray &ray, float max) const {
        for(const Surface *surface : *surfaces){
            float t = surface->hit_test(ray);
            if( t >= 0 && t <= max ){
                return true;
            }
        }

        return false;
    }
};

#endif
