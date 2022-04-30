#pragma once

#include"rtweekend.h"

#include"hittable.h"
#include"hittable_list.h"
#include<algorithm>

class bvh_node :public hittable {
public:
	bvh_node();

	bvh_node(const hittable_list& list,double time0,double time1)
		:bvh_node(list.objects,0,list.objects.size(),time0,time1)
	{}

	bvh_node(
		const std::vector<shared_ptr<hittable>>& src_objects,
		size_t start,size_t end,double time0,double time1);
	
	virtual bool hit(
		const ray& r, double t_min, double t_max, hit_record& rec)const override;

	virtual bool bounding_box(double time0, double time1, aabb& output_box)const override;

public:
	shared_ptr<hittable> left;
	shared_ptr<hittable> right;
	aabb box;
};

bool bvh_node::bounding_box(double time0, double time1, aabb& output_box)const {
	output_box = box;
	return true;
}

inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
	aabb box_a;
	aabb box_b;

	if (!a->bounding_box(0, 0, box_a) || !b->bounding_box(0, 0, box_b))
		std::cerr << "No bounding box in bvh_node constructor.\n";

	return box_a.min().e[axis] < box_b.min().e[axis]; //类排序(升序排列)
}


bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 1);
}

bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 2);
}

bvh_node::bvh_node(
	const std::vector<shared_ptr<hittable>>& src_objects,
	size_t start, size_t end, double time0, double time1
	){
	auto objects = src_objects;//Create a modifiable array of the source scene objects

	int axis = random_int(0, 2); //根据x或y或z的坐标大小随机划分左右子树

	auto comparator = (axis == 0) ? box_x_compare   
		: (axis == 1) ? box_y_compare
		: box_z_compare;

	size_t object_span = end - start;

	if (object_span == 1) {
		left = right = objects[start];
	}
	else if (object_span == 2) {
		if (comparator(objects[start], objects[start + 1])) {
			left = objects[start];
			right = objects[start+1];
		}
		else {
			left = objects[start+1];
			right = objects[start];
		}
	}
	else {
		std::sort(objects.begin() + start, objects.begin() + end, comparator);//比较函数using comparator function as comparison

		auto mid = start + object_span / 2;
		left = make_shared<bvh_node>(objects, start, mid, time0, time1);
		right = make_shared<bvh_node>(objects, mid, end, time0, time1);
	}//排序并划分

	aabb box_left, box_right;

	if (!left->bounding_box(time0, time1, box_left)
		|| !right->bounding_box(time0, time1, box_right)
		)
		std::cerr << "No bounding box in bvh_node constructor.\n";

	box = surrounding_box(box_left, box_right);

	}	

//构建完bvh树后判断相交
bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec)const {
	if (!box.hit(r, t_min, t_max)) return false;
	bool hit_left = left->hit(r, t_min, t_max, rec);
	bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

	return hit_left || hit_right;
}
//bvh_node::hit函数一开始调用aabb类的hit函数（用的是最大的box），但bvh树对象创建完成构造函数就会将整个box构造好。
//当hit到了最大box之后，往下开始判断左子树是否hit到物体：如果是一个叶子节点就直接调用aabb类的hit函数，如果是一个
//bvh_node根节点，那么就会递归调用bvh_node::hit，那么递归的box就是左子树对应的box，递归调用直到出现子叶节点，
//对应节点的box就会调用aabb类的hit，hit到最后的子叶节点后就会调用具体物体的hit函数（如sphere::hit，moving_sphere::hit等）
//并且将最后的hit结果保存起来。再返回到上层节点（true or false),进行右子树的判断，重复这个过程。