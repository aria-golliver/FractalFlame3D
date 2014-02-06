#ifndef __CAMERA
#define __CAMERA

#define CAMERA_DEBUG 0

#ifndef PI
#define PI 3.14159265359
#endif

#include "GenVector.h"
#include "Ray.h"
class Camera {
public:
	Camera(int width, int height):
	r ((float) width),
	l ((float) -width),
	t ((float) height),
	b ((float) -height),
	d ((float) height),
    wid(width),
    hei(height)
    {}

	void setBasis(const Vector3 pos, const Vector3 lookpoint, Vector3 upNormal){
		Vector3 lookDir = (lookpoint - pos);
		upNormal = upNormal.normalize();
		this->w = -lookDir.normalize();
		this->u = (upNormal.cross(this->w)).normalize();
		this->v = (this->w.cross(this->u)).normalize();

		this->e = pos;
	}

	void print() const {
		printf("e: (%f, %f, %f)\n", e.c[0], e.c[1], e.c[2]);
		printf("u: (%f, %f, %f)\n", u.c[0], u.c[1], u.c[2]);
		printf("v: (%f, %f, %f)\n", v.c[0], v.c[1], v.c[2]);
		printf("w: (%f, %f, %f)\n", w.c[0], w.c[1], w.c[2]);
		printf("r: %f, l: %f, t: %f, b: %f, d: %f\n", r, l, t, b, d);
	}

	Ray castRay(const int i, const int j) const {
		if(CAMERA_DEBUG){
			if(i < 0 || j < 0 || i >= this->wid || j >= this->hei){
				printf("castRay: OUT OF BOUNDS %d, %d\n", i, j);
				exit(1);
			}
		}

		const float U = l + (r - l) * (i + 0.5f) / wid;
		const float V = b + (t - b) * (j + 0.5f) / hei;
		const Vector3 dir = ((-d)*w + U*u + V*v).normalize();

		return Ray(this->e, dir);
	}

private:
	const float r;
	const float l;
	const float t;
	const float b;
	
	const float d;

	int wid, hei;

	Vector3 u, v, w;
	Vector3 e;
};

#endif