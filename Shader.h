#ifndef __SHADER
#define __SHADER

#include "GenVector.h"
#include "HitVector.h"
#include "rdrand.h"
#include "SurfaceList.h"
#include <cmath>



void getTotalLight(HitVector& hitVector, 
                          const Surface *nearestSurface,
                          const std::vector<PointLight *> *pointLights, 
                          const std::vector<const Surface *> *surfaces,
                          const SurfaceListAABB &surfaceList) {
	if (!hitVector.hit){
		printf("HOW CAN YOU SHADE SOMETHING WITH NO HIT?!\n");
        hitVector.totalLight = Vector3(100, 0, 0);
		exit(1);
    }

	if (nearestSurface == NULL){
		printf("SHADER: NEAREST SURFACE IS NULL\n");
		hitVector.totalLight = Vector3(0, 100, 0);
		exit(1);
	}

    const Vector3 normal = hitVector.normal;

	const float ambient_scalar = 1.0f;// -acos(abs(normal.dot(hitVector.ray.dir))) / ((float)PI / 2.0f);
    
    Vector3 totalAmbientLight;
    
    
    // compute ray from p to every light
    for(const PointLight *pl : *pointLights){
        // do ambient calculation
        totalAmbientLight += ambient_scalar * pl->amb * nearestSurface->mat_ambient;
    }

    hitVector.totalLight = (totalAmbientLight);

	if (ALPHA > 0 && ALPHA < 1){
		Vector3 totalAlphaLight(0, 0, 0);
		HitVector hitAlpha;
		hitAlpha.hit = false;
		Ray alphaRay = Ray(hitVector.hitPoint - (hitVector.normal * epsilon), hitVector.ray.dir);
		const Surface *nextSurfaceRef = surfaceList.intersectScene(alphaRay, hitAlpha);

		if (hitAlpha.hit){
			getTotalLight(hitAlpha, nextSurfaceRef, pointLights, surfaces, surfaceList);
			hitVector.totalLight += hitAlpha.totalLight * (1.0F - ALPHA);
		}
	}
}

#endif