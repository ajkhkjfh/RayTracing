#pragma once

#include"hittable.h"
#include"vec3.h"
#include"one.h"
#include"pdf.h"

class sphere :public hittable {
public:
	sphere() {}
	sphere(point3 cen, double r,shared_ptr<material> m) :center(cen), radius(r), mat_ptr(m){}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double t_min, double t_max, aabb& output_box) const override;
	virtual double pdf_value(const point3& o, const vec3& v) const override;
	virtual vec3 random(const point3& o) const override;

private:
	static void get_sphere_uv(const point3& p, double& u, double& v) {
		// p: a given point on the sphere of radius one, centered at the origin.
		// u: returned value [0,1] of angle around the Y axis from X=-1.
		// v: returned value [0,1] of angle from Y=-1 to Y=+1.
		//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
		//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
		//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>
		
		
		/*auto theta = acos(-p.y());
		auto phi = atan2(-p.z(), p.x()) + pi;

		u = phi / (2 * pi);
		v = theta / pi;*/

		auto theta = asin(p.y());
		auto phi = atan2(p.z(), p.x()) ;

		u = 1-(phi+pi) / (2 * pi);
		v = (theta+pi/2) / pi;
	}

public:
	point3 center;
	double radius;
	shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec)const {
	vec3 oc = r.origin() - center;
	double a = r.direction().length_squared();
	double half_b = dot(r.direction(), oc);
	double c = oc.length_squared() - radius * radius;
	double discriminant = half_b * half_b - a * c;
	if (discriminant > 0) {
		double root = sqrt(discriminant);
		double t = (-half_b - root) / a;
		if (t > t_min && t < t_max) {
			rec.t = t;
			rec.p = r.at(t);
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r, outward_normal);
			get_sphere_uv(outward_normal, rec.u, rec.v);
			rec.mat_ptr = mat_ptr;//hit_record中的材质指针将设置为在main()中设置该球体时给定的材质指针
			return true;
		}
		t = (-half_b + root) / a;
		if (t > t_min && t < t_max) {
			rec.t = t;
			rec.p = r.at(t);
			vec3 outward_normal = (rec.p - center) / radius;
			rec.set_face_normal(r, outward_normal);
			get_sphere_uv(outward_normal, rec.u, rec.v);
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}

bool sphere::bounding_box(double t_min, double t_max, aabb& output_box) const {
	output_box = aabb(center - vec3(radius, radius, radius),
		center + vec3(radius, radius, radius));
	return true;
}

double sphere::pdf_value(const point3& o, const vec3& v) const {
	hit_record rec;
	if (!this->hit(ray(o, v), 0.001, infinity, rec))
		return 0;

	auto cos_theta_max = sqrt(1 - radius * radius / (center - o).length_squared());
	auto solid_angle = 2 * pi * (1 - cos_theta_max);

	return  1 / solid_angle;
}

vec3 sphere::random(const point3& o) const {
	vec3 direction = center - o;
	auto distance_squared = direction.length_squared();
	onb uvw;
	uvw.build_from_w(direction);
	return uvw.local(random_to_sphere(radius, distance_squared));
}