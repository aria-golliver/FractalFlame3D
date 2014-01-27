#ifndef __SCENE
#define __SCENE

#include "GenVector.h"
#include "PointLight.h"
#include "Surface.h"
#include "Camera.h"
#include <math.h>
#include <vector>

#endif

#pragma once
class Scene {
public:
    std::vector<PointLight *> pointLights;
    std::vector<Surface *> surfaces;
    Camera camera;

    Scene( std::vector<Surface *> surfaces, std::vector<PointLight *> pointLights, Camera camera ): 
        pointLights(pointLights),
        surfaces(surfaces),
        camera(camera)
    {};
};
