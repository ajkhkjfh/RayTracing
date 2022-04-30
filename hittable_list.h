#pragma once

#include "hittable.h"
#include <memory>
#include <vector>
#include "aabb.h"
#include "one.h"

using std::shared_ptr;
using std::make_shared;
using std::vector;

class hittable_list :public hittable {
public:
	hittable_list(){}
	hittable_list(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }
	void add(shared_ptr<hittable> object) { objects.push_back(object); }

	virtual bool hit(const ray& r, double tmin, double tmax, hit_record& rec) const override;
	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;

	virtual double pdf_value(const point3& o, const vec3& v) const override;
	virtual vec3 random(const point3& o) const override;

public:
	vector < shared_ptr<hittable> > objects;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	hit_record temp_rec;
	bool hit_anything = false;
	double closest_so_far = t_max;
	for (const auto& object : objects) { //范围for循环,常引用不能通过引用的变化改变原对象
		if (object->hit(r, t_min, closest_so_far, temp_rec)) {
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}
	return hit_anything;
}

bool hittable_list::bounding_box(double time0, double time1, aabb& output_box) const {
	if (objects.empty()) return false;
	aabb temp_box;
	bool first_box = true;
	for (const auto& object : objects) {
		if (!object->bounding_box(time0, time1, temp_box)) return false; //如果没有包围盒则返回false
		output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);//构造包围盒,如果是第一个包围盒，
		//则用temp——box构造物体的包围盒，当不是第一个包围盒时，用上一个包围盒和该物体的包围盒共同构造包围盒
		first_box = false;
	}
	return true;
}

double hittable_list::pdf_value(const point3& o, const vec3& v) const {
	auto weight = 1.0 / objects.size();
	auto sum = 0.0;

	for (const auto& object : objects)
		sum += weight * object->pdf_value(o, v);

	return sum;
}

vec3 hittable_list::random(const vec3& o) const {
	auto int_size = static_cast<int>(objects.size());
	return objects[random_int(0, int_size - 1)]->random(o);
}