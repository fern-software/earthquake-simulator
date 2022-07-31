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
	
	// Constructs a particle at the given position (pos) which is subjet to the given acceleration (a).
	Particle(Point pos, Vector a = Vector(0, 0)) : pos_(pos), prev_pos_(pos, a_(a)){}

	// Updates the position of the particle using Verlet Integration.
	void update(T dt){
		Point tmp = pos_;
		pos_ += (pos_ - prev_pos_) + (a_ * dt * dt);
		prev_pos_ = tmp;
	}

private:
	template <class T> friend class Joint;
	Point pos_;
	Vector prev_pos_;
	Vector a_;
};

}
