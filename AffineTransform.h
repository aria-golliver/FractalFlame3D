#include "rdrand.h"
#include "GenVector.h"
#ifndef AFFINETRANSFORM
#define AFFINETRANSFORM
class AffineTransform{
public:
	float a, b, c, d, e, f, g, h, i, j, k, l;
	AffineTransform(){
		rdrand_f32(&a);
		rdrand_f32(&b);
		rdrand_f32(&c);
		rdrand_f32(&d);
		rdrand_f32(&e);
		rdrand_f32(&f);
		rdrand_f32(&g);
		rdrand_f32(&h);
		rdrand_f32(&i);
		rdrand_f32(&j);
		rdrand_f32(&k);
		rdrand_f32(&l);
		a = a * 2 - 1;
		b = b * 2 - 1;
		c = c * 2 - 1;
		d = d * 2 - 1;
		e = e * 2 - 1;
		f = f * 2 - 1;
		g = g * 2 - 1;
		h = h * 2 - 1;
		i = i * 2 - 1;
		j = j * 2 - 1;
		k = k * 2 - 1;
		l = l * 2 - 1;

		rdrand_f32(&col[0]);
		rdrand_f32(&col[1]);
		rdrand_f32(&col[2]);

		float maxcol = max3(col[0], col[1], col[2]);
		col /= maxcol;
	}

	Vector3 col;

	Vector3 apply(const Vector3 &p) const {
		float x = (p[0] * a) + (p[1] * b) + (p[2] * c) + (d);
		float y = (p[0] * e) + (p[1] * f) + (p[2] * g) + (h);
		float z = (p[0] * i) + (p[1] * j) + (p[2] * k) + (l);
		return Vector3(x, y, z);
	}
};


#endif