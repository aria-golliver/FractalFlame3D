#ifndef __SURFACELISTAABB
#define __SURFACELISTAABB

#include "Surface.h"
#include "AABBox.h"
#include "HitVector.h"
#include "PointLight.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>
#include <iterator>
#include "SurfaceList.h"

#include <cilk/cilk.h>
#include <cilk/cilk_api_windows.h>

using namespace std;


class SurfaceListAABB : public SurfaceList {
public:
	SurfaceList *LHS, *RHS;
	const Surface *LHS_S, *RHS_S;
	bool left_leaf, right_leaf;

	SurfaceListAABB *factory(const vector<const Surface *> *surfaces) const {
		return new SurfaceListAABB(surfaces);
	}

    SurfaceListAABB(const vector<const Surface *> *surfaces){
		const int size = (int) surfaces->size();
		
		if (size == 0){
			printf("got a zero size surface list, error?!\n");
			exit(1);
		} 

		else if (size == 1){
			left_leaf = true;
			LHS_S = surfaces->at(0);
			LHS = NULL;

			right_leaf = true;
			RHS_S = NULL;
			RHS = NULL;

			AABB = AABBox(LHS_S->min_point, LHS_S->max_point);
		}

		else if (size == 2){
			left_leaf = true;
			LHS_S = surfaces->at(0);
			LHS = NULL;

			right_leaf = true;
			RHS_S = surfaces->at(1);
			RHS = NULL;

			float min_x = min2(LHS_S->min_point[0], RHS_S->min_point[0]);
			float min_y = min2(LHS_S->min_point[1], RHS_S->min_point[1]);
			float min_z = min2(LHS_S->min_point[2], RHS_S->min_point[2]);

			float max_x = max2(LHS_S->max_point[0], RHS_S->max_point[0]);
			float max_y = max2(LHS_S->max_point[1], RHS_S->max_point[1]);
			float max_z = max2(LHS_S->max_point[2], RHS_S->max_point[2]);

			AABB = AABBox(Vector3(min_x, min_y, min_z), Vector3(max_x, max_y, max_z));
		}
		else {
			left_leaf = false;
			right_leaf = false;
			LHS_S = NULL;
			RHS_S = NULL;
			LHS = NULL;
			RHS = NULL;

			vector<const Surface *> surfaces_lhs;
			vector<const Surface *> surfaces_rhs;

			bool no_split_found = true;
			float min_x =  std::numeric_limits<float>::infinity();
			float min_y =  std::numeric_limits<float>::infinity();
			float min_z =  std::numeric_limits<float>::infinity();

			float max_x =  -std::numeric_limits<float>::infinity();
			float max_y =  -std::numeric_limits<float>::infinity();
			float max_z =  -std::numeric_limits<float>::infinity();

			float mean[3] = { 0, 0, 0 };

			for (const Surface *surface : *surfaces){
				min_x = min2(min_x, surface->min_point[0]);
				min_y = min2(min_y, surface->min_point[1]);
				min_z = min2(min_z, surface->min_point[2]);
				
				max_x = max2(max_x, surface->max_point[0]);
				max_y = max2(max_y, surface->max_point[1]);
				max_z = max2(max_z, surface->max_point[2]);

				mean[0] += surface->min_point[0];
				mean[1] += surface->min_point[1];
				mean[2] += surface->min_point[2];
			}

			mean[0] /= size;
			mean[1] /= size;
			mean[2] /= size;

			float dx = abs(max_x - min_x);
			float dy = abs(max_y - min_y);
			float dz = abs(max_z - min_z);
			
			int dir = 0;
			
			if (dx > dy && dx > dz)
				dir = 0;
			if (dy > dx && dy > dz)
				dir = 1;
			if (dz > dx && dz > dy)
				dir = 2;

			int split_tries = 0;
			while (no_split_found) {
				no_split_found = false;
				surfaces_lhs.clear();
				surfaces_rhs.clear();

				for (int i = 0; i < size; i++){
					if (surfaces->at(i)->min_point[dir % 3] < mean[dir % 3])
						surfaces_lhs.push_back(surfaces->at(i));
					
					else
						surfaces_rhs.push_back(surfaces->at(i));
				}

				if (surfaces_lhs.size() == 0 || surfaces_rhs.size() == 0){
					no_split_found = true;
					dir++;
					if (split_tries++ == 3){
						int half_size = size / 2;
						surfaces_lhs.clear();
						surfaces_rhs.clear();
						for (int i = 0; i < half_size; i++){
							surfaces_lhs.push_back(surfaces->at(i));
						}
						for (int i = half_size; i < size; i++){
							surfaces_rhs.push_back(surfaces->at(i));
						}
						no_split_found = false;
					}
				}
			}

			if (surfaces_lhs.size() == 1){
				LHS = NULL;
				LHS_S = surfaces_lhs.at(0);
				left_leaf = true;
			}
			else {
				left_leaf = false;
				LHS_S = NULL;

				if (surfaces_lhs.size() < MIN_TREE_SPLIT)
					LHS = new SurfaceList(new vector<const Surface *>(surfaces_lhs));
				else
					LHS = cilk_spawn factory(&surfaces_lhs);
			}

			if (surfaces_rhs.size() == 1){
				RHS = NULL;
				RHS_S = surfaces_rhs.at(0);
				right_leaf = true;
			}
			else {
				right_leaf = false;
				RHS_S = NULL;

				if (surfaces_rhs.size() < MIN_TREE_SPLIT)
					RHS = new SurfaceList(new vector<const Surface *>(surfaces_rhs));
				else
					RHS = factory(&surfaces_rhs);
			}

			AABB = AABBox(Vector3(min_x, min_y, min_z), Vector3(max_x, max_y, max_z));

			cilk_sync;
		}
    }

    virtual const Surface * intersectScene(const Ray &ray, HitVector &hit) const {
		const Surface *nearestSurface_lhs = NULL;
		const Surface *nearestSurface_rhs = NULL;

		float t_lhs = std::numeric_limits<float>::infinity();
		float t_rhs = std::numeric_limits<float>::infinity();

		HitVector hit_lhs, hit_rhs;
		hit_lhs.hit = false;
		hit_lhs.t = std::numeric_limits<float>::infinity();

		hit_rhs.hit = false;
		hit_rhs.t = std::numeric_limits<float>::infinity();

		if (left_leaf && LHS_S) {
			HitVector hv = LHS_S->hit(ray);
			
			hit_lhs = hv;
			t_lhs = hv.t;
			nearestSurface_lhs = LHS_S;
		}
		else if (!left_leaf && LHS) {
			if (LHS->AABB.hit_test(ray) > 0){
				nearestSurface_lhs = LHS->intersectScene(ray, hit_lhs);
				t_lhs = hit_lhs.t;
			}
		}

		if (right_leaf && RHS_S) {
			HitVector hv = RHS_S->hit(ray);

			hit_rhs = hv;
			t_rhs = hv.t;
			nearestSurface_rhs = RHS_S;
		}
		else if (!right_leaf && RHS) {
			if (t_rhs = RHS->AABB.hit_test(ray) > 0){
				if (t_rhs != -1 && t_rhs < t_lhs || t_lhs == -1){
					nearestSurface_rhs = RHS->intersectScene(ray, hit_rhs);
					t_rhs = hit_rhs.t;
				}
			}
		}

		const Surface *nearestSurface = NULL;
		if (hit_lhs.hit && !hit.hit){
			hit = hit_lhs;
			nearestSurface = nearestSurface_lhs;
		}
		else if (hit_lhs.hit && hit.hit && t_lhs < hit.t){
			hit = hit_lhs;
			nearestSurface = nearestSurface_lhs;
		}

		if (hit_rhs.hit && !hit.hit){
			hit = hit_rhs;
			nearestSurface = nearestSurface_rhs;
		}
		else if (hit_rhs.hit && hit.hit && t_rhs < hit.t){
			hit = hit_rhs;
			nearestSurface = nearestSurface_rhs;
		}

		return nearestSurface;
    }

    /*
     * A much simpler version of intersectScene
     * this function just checkes to see if any objects are closer than [max] distance away from the ray
     * very useful for shadow checking
     */
    virtual bool intersectScene_max(const Ray &ray, const float max) const {
		if (left_leaf && LHS_S) {
			float t = LHS_S->hit_test(ray);
			if (t >= 0 && t <= max)
				return true;
		}

		if (right_leaf && RHS_S) {
			float t = RHS_S->hit_test(ray);
			if (t >= 0 && t <= max)
				return true;
		}

		if (!left_leaf && LHS){
			if (LHS->AABB.hit_test(ray) && LHS->intersectScene_max(ray, max))
				return true;
		}

		if (!right_leaf && RHS){
			if (RHS->AABB.hit_test(ray) &&  RHS->intersectScene_max(ray, max))
				return true;
		}

		return false;
    }

	virtual int size() const {
		int items = 0;
		if (left_leaf && LHS_S)
			items++;
		else if (LHS)
			items += LHS->size();

		if (right_leaf && RHS_S)
			items++;
		else if (RHS)
			items += RHS->size();

		return items;
	}

	virtual void print(){
		int LHS_size = 0;
		int RHS_size = 0;

		if (left_leaf && LHS_S)
			LHS_size++;

		if (right_leaf && RHS_S)
			RHS_size++;

		if (LHS)
			LHS_size += LHS->size();
		if (RHS)
			RHS_size += RHS->size();

		printf("%d %d\n", LHS_size, RHS_size);
		if (LHS)
		LHS->print();
		if (RHS)
		RHS->print();
	}
};

#endif
