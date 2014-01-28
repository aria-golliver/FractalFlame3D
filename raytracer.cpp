#define fractal_points  10000000
#define MIN_TREE_SPLIT (10)
#define radius 0.003
#define ALPHA 0.01f

int wid = 1920, hei = 1080;

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
#include "Variations.h"

using namespace std;
using namespace std::chrono;

Vector3 obj_to_Vector3(const obj_vector* v);
void createFractal(vector<const Surface *> &surfaces);



#include "Shader.h"

const int num_threads = 8;

#define n_affines 3
AffineTransform affs[n_affines];

int main(int argc, char **argv){
	srand(time(NULL));
	printf("THIS MUST BE RUN ON AN IVY BRIDGE CPU OR BETTER!!!\n");
	omp_set_num_threads(num_threads);
	string filename = "fractals";

	high_resolution_clock::time_point time_start = high_resolution_clock::now();


	vector<const Surface *> surfaces;

	for (int i = 0; i < n_affines; i++)
		affs[i] = AffineTransform();


	createFractal(surfaces);

	high_resolution_clock::time_point surfacelist_time_start = high_resolution_clock::now();

	SurfaceListAABB surfaceList(&surfaces);

	high_resolution_clock::time_point surfacelist_time_end = high_resolution_clock::now();
	duration<double> surfacelist_runtime = duration_cast<duration<double>>(surfacelist_time_end - surfacelist_time_start);
	printf("time to create surface tree: %.2f\n", surfacelist_runtime.count());


	for (int framecount = 0; framecount < 100; framecount++){
		high_resolution_clock::time_point frame_start = high_resolution_clock::now();
		Buffer b = Buffer(wid, hei);
		LightBuffer lb = LightBuffer(wid, hei);

		float theta = framecount * PI * 2.0f / 100.0f;

		vector<PointLight *> pointLights;
		Camera camera(wid, hei);
		pointLights.push_back(new PointLight(Vector3(2, 2, 2), Vector3(2, 2, 2), Vector3(2, 2, 2), Vector3(2, 2, 2)));

		camera.setBasis(Vector3(4 * sin(theta), 0, 4 * cos(theta)), Vector3(0, 0, 0), Vector3(0, 1, 0));

		int linesDrawn = 0;
		int old_percdone = 0;
#pragma omp parallel for schedule(dynamic)
		for (int j = 0; j < hei; j++){

#define LOADING_BAR
#ifdef  LOADING_BAR
#pragma omp critical
			{
				float perc_done = 100.0f * linesDrawn++ / (float)hei;
				printf("\r%2.0f ", perc_done);
				if ((int)perc_done > old_percdone){
					old_percdone = (int)perc_done;
					for (int k = 0; k < (int)(perc_done / 2); k++){
						putchar('=');
					}
					for (int k = 0; k < 50 - (int)(perc_done / 2) - 1; k++){
						putchar('-');
					}
					fflush(stdout);
				}
			}
#endif

			for (int i = 0; i < wid; i++){
				Ray ray = camera.castRay(i, j);

				Vector3 c(0, 0, 0);

				HitVector hit;
				hit.hit = false;
				hit.t = std::numeric_limits<float>::infinity();

				const Surface *nearestSurface = surfaceList.intersectScene(ray, hit);

				if (hit.hit){
					getTotalLight(hit, nearestSurface, &pointLights, &surfaces, surfaceList);

					c = hit.totalLight;
				}

				lb.at(i, hei - j - 1) = c;
			}
		}

		//printf("\n");

		float m = 0;
		for (int j = 0; j < hei; j++)
		for (int i = 0; i < wid; i++){
			// get max from lb
			Vector3 light = lb.at(i, j);
			m = max4(m, light[0], light[1], light[2]);
		}

		m = log(m);
		// scale all of lb, save to b
		// save b

		#pragma omp parallel for
		for (int j = 0; j < hei; j++)
		for (int i = 0; i < wid; i++){
			Vector3 scaled = lb.at(i, j);

			scaled = (scaled.length() > 0) ? Vector3(log(scaled[0]), log(scaled[1]), log(scaled[2])) : Vector3(0 , 0, 0);
			scaled /= m;
			scaled *= 0xFF;
			
			Color c = Color((unsigned char)abs(scaled[0]), (unsigned char)abs(scaled[1]), (unsigned char)abs(scaled[2]));
			b.at(i, j) = c;
		}

		char frame_string[10];
		sprintf(frame_string, "%05d", framecount);
		simplePPM_write_ppm((filename + string(frame_string) + string(".ppm")).c_str(), wid, hei, (unsigned char*)&b.at(0, 0));
	

		high_resolution_clock::time_point frame_end = high_resolution_clock::now();
		duration<double> frametime = duration_cast<duration<double>>(frame_end - frame_start);
		printf("| time for frame #%3d: %3.2f | \n", framecount, frametime.count());
	}
	
	high_resolution_clock::time_point time_end = high_resolution_clock::now();
	duration<double> runtime = duration_cast<duration<double>>(time_end - time_start);
	printf("\ntime to render: %f\n", runtime.count());
	getchar();

	return 0;
}

Vector3 obj_to_Vector3(obj_vector const* v){
	return Vector3((float)v->e[0], (float)v->e[1], (float)v->e[2]);
}

void createFractal(vector<const Surface *> &surfaces) {
	high_resolution_clock::time_point fractal_time_start = high_resolution_clock::now();

#pragma omp parallel for schedule(dynamic)
	for (int k = 0; k < num_threads; k++) {
		Vector3 p(0, 0, 0);
		Vector3 col(0, 0, 0);
		while (surfaces.size() != fractal_points){
			unsigned int idx;
			rdrand_u32(&idx);
			idx %= n_affines;

			AffineTransform &aff = affs[idx];
			p = aff.apply(p);

			p = v3z(p);

			col += aff.col;
			col /= 2;

#pragma omp critical
			{
				if (surfaces.size() < fractal_points)
					surfaces.push_back(new Sphere(p, col));
			}
		}
	}


	high_resolution_clock::time_point fractal_time_end = high_resolution_clock::now();
	duration<double> fractal_runtime = duration_cast<duration<double>>(fractal_time_end - fractal_time_start);

	printf("%d fractal points generated in %f seconds\n", surfaces.size(), fractal_runtime.count());
}