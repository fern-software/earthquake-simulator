#pragma once

#include <CGAL/Cartesian.h>
#include <CGAL/Point_2.h>

typedef CGAL::Point_2<CGAL::Cartesian<double>> CGAL_Point_2;


// Extend CGAL::Point_2 to include a mass and acceleration
class Point : public CGAL::Point_2<CGAL::Cartesian<double>> {
	public:
		Point(double x, double y, double mass) : CGAL::Point_2<CGAL::Cartesian<double>>(x, y), mass_(mass), accel_x_(0), accel_y_(0) {}
	private:
		double mass_;
		double accel_x_;
		double accel_y_;
};
