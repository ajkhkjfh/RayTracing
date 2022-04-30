#ifndef COLOR_H
#define COLOR_H

#include"vec3.h"
#include<iostream>

void write_color(std::ostream& out, color pixel_color,int samples_per_pixel) {
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	// Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
	if (r != r) r = 0.0;
	if (g != g) g = 0.0;
	if (b != b) b = 0.0;
	//根据样本数对颜色取平均值
	auto scale = 1.0 / samples_per_pixel;
	r = sqrt(r*scale);
	g = sqrt(g*scale);
	b = sqrt(b*scale);

	//写下每个颜色分量转化后的值[0,255]
	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}

#endif


