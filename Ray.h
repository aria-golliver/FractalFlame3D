#ifndef __RAY
#define __RAY

// to do, finish rays
// in camera, finish casting rays

class Ray {
public:
	Vector3 pos, dir, inv;

    Ray(){};

	Ray(Vector3 pos, Vector3 dir): 
        pos(pos),
        dir(dir),
        inv(1.0f / dir)
    {};
	
	void print(){
		pos.print();
		dir.print();
	}
    
    Ray &operator=(const Ray &r){
        this->pos = r.pos;
        this->dir = r.dir;
        this->inv = r.inv;

        return *this;
    }

};

#endif