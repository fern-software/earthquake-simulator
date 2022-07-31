#pragma once

#include <CGAL/Point_2.h>


// Extend CGAL::Point_2 to include a mass and acceleration
template <typename T>
class Point : public CGAL::Point_2<T> {
	public:
		Point(T x, T y, T mass) : CGAL::Point_2<T>(x, y), mass_(mass), accel_x_(0), accel_y_(0) {}
	private:
		T mass_;
		T accel_x_;
		T accel_y_;
};
// template<typename T> class Point : public CGAL::Point_2<T> {
// public:
// 	// initializes a point with the given coordinates, a mass, and an acceleration of 0
// 	Point(T x, T y, T mass) : CGAL::Point_2<T>(x, y), mass_(mass), accel_x_(0), accel_y(0){}

// private:
// 	T mass_;
// 	T accel_x_;
// 	T accel_y_;
// };
