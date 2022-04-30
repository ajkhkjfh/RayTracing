#pragma once

#include"vec3.h"

class ray {
public:
	ray(){}
	ray(const point3& origin,const vec3& direction,double time=0.0):orig(origin),dir(direction),tm(time){}//或者 orig = origin, dir = direction

	point3 origin() const { return orig; } //常量成员函数，隐式this指针类型是指向常量的常量指针
	vec3 direction() const { return dir; }
	double time()const { return tm; }

	point3 at(double t) const {
		return orig + t * dir;
	}

public:
	point3 orig;
	vec3 dir;
	double tm;
};
