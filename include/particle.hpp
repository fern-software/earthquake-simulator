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
	
	// Constructs a particle at the given position (pos) which is subjet to the given acceleration (a).
	Particle(Point pos, Vector a = Vector(0, 0)) : pos_(pos), prev_pos_(pos), a_(a){}

	// Updates the position of the particle using Verlet Integration.
	void update(T dt){
		Point tmp = pos_;
		pos_ += (pos_ - prev_pos_) + (a_ * dt * dt);
		prev_pos_ = tmp;
	}

	// Moves the particle by the given vector (v).
	// void move(Vector v){
	// 	pos_ += v;
	// }

	// Returns the position of the particle.
	Point pos() const { return pos_; }

private:
	template <class U> friend class Joint;
	Point pos_;
	Point prev_pos_;
	Vector a_;
};

}
