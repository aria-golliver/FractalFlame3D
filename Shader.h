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
                          const std::vector<Surface *> *surfaces,
                          const SurfaceListAABB &surfaceList,
                          unsigned int reflections,
                          unsigned int &rngindex,
                          const float *rng_buffer) {
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
    Vector3 totalDiffuseLight;
    Vector3 totalSpecLight;
    
    
    // compute ray from p to every light
    for(const PointLight *pl : *pointLights){
        const Vector3 lineToLight = pl->pos - hitVector.hitPoint;
        
        int numShadows = 0;

		for (unsigned int k = 0; k < SHADOW_DETAIL; k++){
			float dx = 0, dy = 0, dz = 0, dr;
			Vector3 randOffset(0, 0, 0);

			if (SHADOW_DETAIL > 1){
				if (SHADOW_DIFFUSAL_FACTOR != 0){
					rngindex %= RNG_BUFF_SIZE - 4;

					dx = rng_buffer[rngindex++];
					if (MODULATE_SHADOW_Y) dy = rng_buffer[rngindex++];
					dz = rng_buffer[rngindex++];
					dr = rng_buffer[rngindex++];
					randOffset = dr * Vector3(dx, dy, dz).normalize() * SHADOW_DIFFUSAL_FACTOR;
				}

				const Vector3 lineToSoftLight = lineToLight + randOffset;

				const Ray softRay = Ray(hitVector.hitPoint + (hitVector.normal * epsilon), lineToSoftLight);
				if (surfaceList.intersectScene_max(softRay, lineToSoftLight.length())){
					numShadows++;
				}
			}
        }

        // do ambient calculation
        totalAmbientLight += ambient_scalar * pl->amb;

        // if every shadow ray intersected an object, the object is fully in shadow,
        // and you can ignore all these values
        if(numShadows != SHADOW_DETAIL){
            const Ray ray_to_light(hitVector.hitPoint + (hitVector.normal * epsilon), lineToLight);

            const float shadowScalar = 1.0f - (float)numShadows / SHADOW_DETAIL;
            
            // do diffuse calculation
            totalDiffuseLight += shadowScalar * pl->diff * abs(ray_to_light.dir.dot(normal));
            
            // do specular calculation
            if(nearestSurface->shiny != 0){
                const Vector3 lr = ((2 * (ray_to_light.dir.dot(normal)) * normal) - ray_to_light.dir).normalize();

                const float dotProd = (-hitVector.ray.dir).dot(lr);
                if(dotProd > 0){
                    const Vector3 specLight = powf(dotProd, nearestSurface->shiny) * pl->spec;
                        
                    totalSpecLight += shadowScalar * specLight * pl->spec;
                }
            }
        }
    }

    hitVector.totalLight =   (totalAmbientLight * nearestSurface->mat_ambient)
                           + (totalDiffuseLight * nearestSurface->mat_diffuse)
                           +    (totalSpecLight * nearestSurface->mat_specular);

     // if this is the first application of the shader we want to use the maximum number of reflection rays
    // else we want to use only 1 (to make things faster)
    unsigned int num_soft_reflections = (reflections == MAX_REFLECTIONS) ? REFLECTION_DETAIL : 1;

    // but if we have reflected MAX_REFLECTIONS times (therefore reflections = 0), then cast no reflection rays
    num_soft_reflections = (reflections == 0) ? 0 : num_soft_reflections;

    // if the surface isn't reflective, give up
    if(nearestSurface->reflect > 0 && num_soft_reflections >= 1){
        Vector3 totalReflectedLight(0, 0, 0);

        Ray reflectedRay = Ray(hitVector.hitPoint + (hitVector.normal * epsilon), hitVector.ray.dir.reflect(hitVector.normal));
        for(unsigned int k = 0; k < num_soft_reflections; k++) {
            float dx = 0, dy = 0, dz = 0, dr = 0; 
            Vector3 randOffset(0,0,0);

            // if there is more than 1 soft reflection, randomly modulate the angle of the ray
            // if there is only 1 soft reflection, then just generate the normal, perfect ray
            if(num_soft_reflections > 1 && REFLECTION_DIFFUSAL_FACTOR != 0) {
                rngindex %= RNG_BUFF_SIZE - 4;
                dx = rng_buffer[rngindex++];
                dy = rng_buffer[rngindex++];
                dz = rng_buffer[rngindex++];
                dr = rng_buffer[rngindex++];
                randOffset = dr * Vector3(dx, dy, dz).normalize();
            }

            Ray reflectedRaySoft = Ray(reflectedRay.pos, (reflectedRay.dir * REFLECTION_DIFFUSAL_FACTOR) + randOffset);

            HitVector hitReflect;
            hitReflect.hit = false;

            const Surface *nearestSurfaceRef = surfaceList.intersectScene(reflectedRaySoft, hitReflect);

            if(hitReflect.hit){
                getTotalLight(hitReflect, nearestSurfaceRef, pointLights, surfaces, surfaceList, reflections - 1, rngindex, rng_buffer);
                totalReflectedLight += hitReflect.totalLight * nearestSurface->reflect;
            }
        }
        hitVector.totalLight += totalReflectedLight * (1.0f / num_soft_reflections);
    }

	if (nearestSurface->alpha > 0){
		Vector3 totalAlphaLight(0, 0, 0);
		HitVector hitAlpha;
		hitAlpha.hit = false;
		Ray alphaRay = Ray(hitVector.hitPoint - (hitVector.normal * epsilon), hitVector.ray.dir);
		const Surface *nextSurfaceRef = surfaceList.intersectScene(alphaRay, hitAlpha);

		if (hitAlpha.hit){
			getTotalLight(hitAlpha, nextSurfaceRef, pointLights, surfaces, surfaceList, reflections, rngindex, rng_buffer);
			hitVector.totalLight += hitAlpha.totalLight * nearestSurface->alpha;
		}
	}
}

#endif