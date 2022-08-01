#pragma once

#include <CGAL/Cartesian.h>
#include <CGAL/Point_2.h>
#include <CGAL/Vector_2.h>

namespace physics {

// Represents a 2D particle who's position is updated using Verlet Integration.
// As of now there is no support for particles of different masses but that
// could be added in the future, without much difficulty.
template <class T> class Particle {
public:
	using Point = CGAL::Point_2<CGAL::Cartesian<T>>;
	using Vector = CGAL::Vector_2<CGAL::Cartesian<T>>;
	
	// Constructs a particle at the given position which is subjet to the given acceleration.
	Particle(T x, T y, Vector a) : pos_(Point(x, y)), prev_pos_(Point(x, y)), a_(a){}

	// Updates the position of the particle using Verlet Integration.
	void update(T dt){
		Point tmp = pos_;
		pos_ += (pos_ - prev_pos_) + (a_ * dt * dt);
		prev_pos_ = tmp;
	}

	T x() const {
		return pos_.x();
	}

	T y() const {
		return pos_.y();
	}

private:
	template <class U> friend class Joint;
	Point pos_;
	Point prev_pos_;
	Vector a_;
};

}
