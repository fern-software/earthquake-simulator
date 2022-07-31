#pragma once

#include <CGAL/Point_2.h>
#include <CGAL/Vector_2.h>

namespace physics {

// Represents a 2D particle who's position is updated using Verlet Integration.
// As of now there is no support for particle's of different masses but that
// could be added in the future, without much difficulty.
template <class T> class Particle {
public:
	using Point = CGAL::Point_2<Cartesian<T>>;
	using Vector = CGAL::Vector_2<Cartesian<T>>;
	
	Particle(T x, T y) : position_(Point(x, y)), prev_position_(Point(x, y)){}

private:
	Point position_;
	Vector prev_position_;
	Vector acceleration_;
};

}
