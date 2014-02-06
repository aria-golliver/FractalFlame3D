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
		exit(1);
    }

	if (nearestSurface == NULL){
		printf("SHADER: NEAREST SURFACE IS NULL\n");
		exit(1);
	}
    
	// add in the alpha component
	hitVector.totalLight = nearestSurface->mat_ambient;

#ifdef ALPHA
	HitVector hitAlpha;
	hitAlpha.hit = false;
	Ray alphaRay = Ray(hitVector.hitPoint - (hitVector.normal * epsilon), hitVector.ray.dir);
	const Surface *nextSurfaceRef = surfaceList.intersectScene(alphaRay, hitAlpha);
	
	if (hitAlpha.hit){
		getTotalLight(hitAlpha, nextSurfaceRef, pointLights, surfaces, surfaceList);
		hitVector.totalLight += hitAlpha.totalLight;
	}
#endif
}

#endif