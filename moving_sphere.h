#pragma once

#include"rtweekend.h"
#include "hittable.h"
#include"aabb.h"
class moving_sphere :public hittable
{
public:
	moving_sphere(){}
	moving_sphere(point3 c0, point3 c1, double t0, double t1, double r, shared_ptr<material> m):
		center0(c0),center1(c1),time0(t0),time1(t1),radius(r),mat_ptr(m){}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const override;

	point3 center(double time) const; //const常量成员函数不能改变调用它的对象的内容（即成员）

	
public:
	point3 center0, center1;
	double radius;
	shared_ptr<material> mat_ptr;
	double time0, time1;
};

point3 moving_sphere::center(double time) const {
	return center0 + (time - time0) / (time1 - time0) * (center1-center0);
}

bool moving_sphere:: hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	vec3 oc = r.origin() - center(r.time());
	double a = r.direction().length_squared();
	double half_b = dot(oc, r.direction());
	double c = dot(oc, oc) - radius * radius;
	double discriminant = half_b * half_b - a * c;
	if (discriminant > 0) {
		double sqrtd = sqrt(discriminant);
		double t = (-half_b - sqrtd) / a;
		if (t > t_min && t < t_max) {
			rec.t = t;
			rec.p = r.at(t);
			vec3 outward_normal = (rec.p - center(r.time()))/radius;
			rec.set_face_normal(r, outward_normal);
			rec.mat_ptr = mat_ptr;
			return true;
		}
		t = (-half_b + sqrtd) / a;
		if (t > t_min && t < t_max) {
			rec.t = t;
			rec.p = r.at(t);
			vec3 outward_normal = (rec.p - center(r.time())) / radius;
			rec.set_face_normal(r, outward_normal);
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

bool moving_sphere::bounding_box(double _time0, double _time1, aabb& output_box) const {
	aabb box0 = aabb(center(time0) - vec3(radius, radius, radius),
		center(time0) + vec3(radius, radius, radius));
	aabb box1 = aabb(center(time1) - vec3(radius, radius, radius),
		center(time1) + vec3(radius, radius, radius));
	output_box = surrounding_box(box0, box1);
	return true;

}