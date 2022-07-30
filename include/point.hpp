#pragma once

#include <CGAL/Point_2.h>

template<typename T> Point : public CGAL::Point_2<T> {
public:
	// initializes a point with the given coordinates, a mass, and an acceleration of 0
	Point(T x, T y, T mass) : CGAL::Point_2<T>(x, y), mass_(mass), accel_x_(0), accel_y(0){}

private:
	T mass_;
	T accel_x_;
	T accel_y_;
};
