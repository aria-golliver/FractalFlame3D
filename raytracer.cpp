#include "GenVector.h"
#include "Buffer.h"
#include "objLoader.h"
#include "Camera.h"
#include "Sphere.h"
#include "Triangle.h"
#include "AABBox.h"
#include "PointLight.h"
#include "rdrand.h"
#include "SurfaceList.h"
#include "SurfaceListAABB.h"
#include <vector>
#include <chrono>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>
#include <random>
#include <omp.h>
#include "AffineTransform.h"

using namespace std;
using namespace std::chrono;

Vector3 obj_to_Vector3(const obj_vector* v);
void createFractal(vector<Surface *> &surfaces);


int wid = 1080, hei = 1080;

const unsigned int SHADOW_DETAIL = 0;
const float SHADOW_DIFFUSAL_FACTOR = 1;
const int MODULATE_SHADOW_Y = 1;

const int REFLECTION_DETAIL = 1;
const float REFLECTION_DIFFUSAL_FACTOR = 1;
const unsigned int MAX_REFLECTIONS = 0;


float *rng;
int RNG_BUFF_SIZE = 64;

#include "Shader.h"

const int num_threads = 8;

#define n_affines 6
AffineTransform affs[n_affines];

int main(int argc, char **argv){
	//srand(time(NULL));
	printf("THIS MUST BE RUN ON AN IVY BRIDGE CPU OR BETTER!!!\n");
	omp_set_num_threads(num_threads);
	string filename = "fractals.obj";

	// create the RNG buffer
	high_resolution_clock::time_point rng_time_start = high_resolution_clock::now();
	rng = (float *)malloc(RNG_BUFF_SIZE * sizeof(float));

	for (int i = 0; i < RNG_BUFF_SIZE; i++){
		rdrand_f32(rng + i);
	}

	high_resolution_clock::time_point rng_time_end = high_resolution_clock::now();
	duration<double> rng_runtime = duration_cast<duration<double>>(rng_time_end - rng_time_start);
	printf("time to create rng: %f\n", rng_runtime.count());





	// Sphere(posvec, r, avec, dvec, svec, shiny, reflect)
	// PointLight(posvec, avec, dvec, svec)
	vector<Surface *> surfaces;

	for (int i = 0; i < n_affines; i++)
		affs[i] = AffineTransform();

	createFractal(surfaces);

	SurfaceListAABB surfaceList(&surfaces);
	//surfaceList.print();
	printf("%d %d surfaces total\n", (int)surfaces.size(), (int)surfaceList.size());

	high_resolution_clock::time_point time_start = high_resolution_clock::now();

	for (int framecount = 0; framecount < 100; framecount++){
		high_resolution_clock::time_point frame_start = high_resolution_clock::now();
		Buffer b = Buffer(wid, hei);
		LightBuffer lb = LightBuffer(wid, hei);

		float theta = framecount * PI * 2.0f / 100.0f;
		// printf("\n---------\n\n");
		printf("frame theta %f %d\n", theta, framecount);

		vector<PointLight *> pointLights;
		Camera camera(wid, hei);
		pointLights.push_back(new PointLight(Vector3(2, 2, 2), Vector3(2, 2, 2), Vector3(2, 2, 2), Vector3(2, 2, 2)));
		//pointLights.push_back(new PointLight(Vector3(2 * sin(theta), 0, 2 * cos(theta)), Vector3(1, 1, 1), Vector3(1, 1, 1), Vector3(1, 1, 1)));
		camera.setBasis(Vector3(2 * sin(theta), 0, 2 * cos(theta)), Vector3(0, 0, 0), Vector3(0, 1, 0));
		//camera.print();

		int linesDrawn = 0;
#pragma omp parallel for schedule(dynamic)
		for (int j = 0; j < hei; j++){

#define LOADING_BAR
#ifdef  LOADING_BAR
#pragma omp critical
			{
				float perc_done = 100.0f * linesDrawn++ / (float)hei;
				printf("\r%2.0f ", perc_done);
				for (int k = 0; k < (int)(perc_done / 2); k++){
					putchar('=');
				}
				for (int k = 0; k < 50 - (int)(perc_done / 2) - 1; k++){
					putchar('-');
				}
				fflush(stdout);
			}
#endif
			unsigned int rngindex;
			rdrand_u32(&rngindex);
			rngindex = rngindex % (RNG_BUFF_SIZE / 2);

			for (int i = 0; i < wid; i++){
				Ray ray = camera.castRay(i, j);

				//Vector3 c(abs(ray.dir[0]), abs(ray.dir[1]), abs(ray.dir[2]));
				Vector3 c(0, 0, 0);

				HitVector hit;
				hit.hit = false;
				hit.t = std::numeric_limits<float>::infinity();

				Surface *nearestSurface = surfaceList.intersectScene(ray, hit);

				if (hit.hit){
					rngindex = rngindex % RNG_BUFF_SIZE;
					getTotalLight(hit, nearestSurface, &pointLights, &surfaces, surfaceList, MAX_REFLECTIONS, rngindex, rng);

					c = hit.totalLight;
				}

				lb.at(i, hei - j - 1) = c;
			}
		}

		printf("\n");

		float m = 0;
		for (int j = 0; j < hei; j++)
		for (int i = 0; i < wid; i++){
			// get max from lb
			Vector3 light = lb.at(i, j);
			m = max4(m, light[0], light[1], light[2]);
		}

		// scale all of lb, save to b
		// save b

		#pragma omp parallel for
		for (int j = 0; j < hei; j++)
		for (int i = 0; i < wid; i++){
			Vector3 scaled = lb.at(i, j) * (0xFF / m);
			Color c = Color((unsigned char)abs(scaled[0]), (unsigned char)abs(scaled[1]), (unsigned char)abs(scaled[2]));
			b.at(i, j) = c;
		}

		char frame_string[10];
		sprintf(frame_string, "%05d", framecount);
		simplePPM_write_ppm((filename + string(frame_string) + string(".ppm")).c_str(), wid, hei, (unsigned char*)&b.at(0, 0));
	

		high_resolution_clock::time_point frame_end = high_resolution_clock::now();
		duration<double> frametime = duration_cast<duration<double>>(frame_end - frame_start);
		printf("time for frame: %f\n", frametime.count());
	}
	
	high_resolution_clock::time_point time_end = high_resolution_clock::now();
	duration<double> runtime = duration_cast<duration<double>>(time_end - time_start);
	printf("time to render: %f\n", runtime.count());
	getchar();

	return 0;
}

Vector3 obj_to_Vector3(obj_vector const* v){
	return Vector3((float)v->e[0], (float)v->e[1], (float)v->e[2]);
}

#define fractal_points 1000000
void createFractal(vector<Surface *> &surfaces){
#pragma omp parallel for schedule(dynamic)
	for (int k = 0; k < num_threads; k++) {
		Vector3 p(0, 0, 0);
		Vector3 col(0, 0, 0);
		for (int i = 0; i < fractal_points / num_threads; i++){
			unsigned int idx;
			rdrand_u32(&idx);
			idx %= n_affines;

			AffineTransform &aff = affs[idx];
			p = aff.apply(p);

			float x = sin(p[0]);
			float y = sin(p[1]);
			float z = sin(p[2]);

			p = Vector3(x, y, z);

			col += aff.col;
			col /= 2;

#pragma omp critical
			surfaces.push_back(new Sphere(
				Vector3(x, y, z), 0.001f,
				col,
				col * 3,
				col, 10, 0, 0.1));
		}
	}

	printf("%d fractal points generated\n", surfaces.size());
}