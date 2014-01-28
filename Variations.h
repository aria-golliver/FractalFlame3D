#ifndef VARIATIONS
#define VARIATIONS

#include "GenVector.h"
#include <cmath>

inline Vector3 v1(Vector3 p) {
	float x = sin(p[0]);
	float y = sin(p[1]);
	float z = sin(p[2]);

	return Vector3(x, y, z);
}

inline Vector3 v2(Vector3 p) {
	float rsq = p.squaredLength();
	float x = p[0] * (1.0f / rsq);
	float y = p[1] * (1.0f / rsq);
	float z = p[2] * (1.0f / rsq);

	return Vector3(x, y, z);
}

inline Vector3 v3z(Vector3 p) {
	float rsq = p.squaredLength();
	float x = p[0] * sin(rsq) - p[1] * cos(rsq);
	float y = p[0] * cos(rsq) + p[1] * sin(rsq);
	float z = p[2];

	return Vector3(x, y, z);
}

inline Vector3 v3y(Vector3 p) {
	float rsq = p.squaredLength();
	float x = p[0] * sin(rsq) - p[2] * cos(rsq);
	float y = p[1];
	float z = p[0] * sin(rsq) + p[2] * cos(rsq);

	return Vector3(x, y, z);
}

inline Vector3 v3x(Vector3 p) {
	float rsq = p.squaredLength();
	float x = p[0];
	float y = p[1] * sin(rsq) - p[2] * cos(rsq);
	float z = p[1] * sin(rsq) + p[2] * cos(rsq);

	return Vector3(x, y, z);
}

#endif
