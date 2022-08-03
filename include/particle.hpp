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
	Particle(T x, T y, bool fixed, unsigned int system_width, unsigned int system_height, Vector a) :
		pos_(Point(x, y)),
		prev_pos_(Point(x, y)),
		a_(a),
		system_width_(system_width),
		system_height_(system_height),
		fixed_(fixed){}

	bool operator==(const Particle& other) const {
		return pos_ == other.pos_;
	}
	
	// Updates the position of the particle using Verlet Integration.
	void update(T dt){
		if(fixed_){
			return;
		}

		Point tmp = pos_;
		pos_ += (pos_ - prev_pos_) + (a_ * dt * dt);
		prev_pos_ = tmp;

		stay_in_bounds();
	}

	// Makes sure that the particle is within ([0, width], [0, height])
	void stay_in_bounds(){
		// check x boudaries
		if(pos_.x() < 0){
			pos_ = Point(0, pos_.y());
		}
		else if(pos_.x() > system_width_){
			pos_ = Point(system_width_, pos_.y());
		}

		// check y boundaries
		if(pos_.y() < 0){
			pos_ = Point(pos_.x(), 0);
		}
		else if(pos_.y() > system_height_){
			pos_ = Point(pos_.x(), system_height_);
		}
	}

	// Moves the particle a given distance. Ignores if a particle is fixed or not but does make
	// sure that the particle stays within the bounds of the system.
	void move(T dx, T dy){
		pos_ += Vector(dx, dy);
		stay_in_bounds();
	}

	bool fixed() const {
		return fixed_;
	}

	T x() const {
		return pos_.x();
	}

	T y() const {
		return pos_.y();
	}

private:
	// Joints need to be able to directly modify the position of particles
	template <class U> friend class Joint;

	// position of the particle
	Point pos_;

	// previous position of the particle
	Point prev_pos_;

	// constant accleration the particle is subject to
	Vector a_;

	// width and height of the system the particle is in
	unsigned int system_width_;
	unsigned int system_height_;

	// whether the particle is fixed to a point in space or not
	bool fixed_;
};

}
