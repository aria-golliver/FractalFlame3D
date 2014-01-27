#ifndef __BUFFER
#define __BUFFER

#include "genVector.h"
#include "simplePPM.h"
class Buffer{
public:
	Buffer(unsigned int w, unsigned int h){
		this->w = w;
		this->h = h;

		alloc();
	}

	Color at(unsigned int x, unsigned int y) const {
		return this->buffer[x + (y * this->w)];
	}

	Color & at(unsigned int x, unsigned int y){
		return this->buffer[x + (y * this->w)];
	}

private:
	unsigned int w, h;
	Color *buffer;
	void alloc(){
		buffer = (Color *) malloc(this->w * this->h * sizeof(Color));
        for(unsigned int i = 0; i < this->w * this->h; i++){
            buffer[i] = Color(0, 0, 0);
        }
	}
};


class LightBuffer{
public:
	LightBuffer(unsigned int w, unsigned int h){
		this->w = w;
		this->h = h;

		alloc();
	}

	Vector3 at(unsigned int x, unsigned int y) const {
		return this->buffer[x + (y * this->w)];
	}

	Vector3 & at(unsigned int x, unsigned int y){
		return this->buffer[x + (y * this->w)];
	}

private:
	unsigned int w;
    unsigned int h;
	Vector3 *buffer;
	void alloc(){
		buffer = (Vector3 *) malloc(this->w * this->h * sizeof(Vector3));
        for(unsigned int i = 0; i < this->w * this->h; i++){
            buffer[i] = Vector3(0, 0, 0);
        }
	}
};

#endif