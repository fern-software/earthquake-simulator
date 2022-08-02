#pragma once

#include <cmath>

#include "particle.hpp"

namespace physics {

// Represents a Joint between two particles who's length is fixed.
template <class T> class Joint {
public:
	// The particle type used by the joint. Its important that its a reference type as a 
	// joint never owns a particle as many joints may be connected to the same particle.
	using ParticleType = Particle<T>&;
	using Point = typename Particle<T>::Point;
	using Vector = typename Particle<T>::Vector;

	// Constructs a joint between the given particles. The length of the joint is set to
	// the distance between the two particles at the time of creation.
	Joint(ParticleType p1, ParticleType p2) : p1_(p1), p2_(p2){
		length_ = std::sqrt((p1.pos_ - p2.pos_).squared_length());
	}

	// Maintains the length of the joint by moving the two particles closer or farther apart
	// depending on the current distance between them. Depending on the number of joints
	// connected to p1 and p2, multiple iterations of this function may be necessary.
	void maintain_length(){
		// computes the current distance between the two particles
		Vector delta = p2_.pos_ - p1_.pos_;
		T distance = std::sqrt(delta.squared_length());
		T diff = (distance - length_) / distance;

		// updates the position of the particles
		p1_.pos_ += delta * 0.5 * diff;
		p2_.pos_ -= delta * 0.5 * diff;
	}

	T x1() const {
		return p1_.x();
	}

	T y1() const {
		return p1_.y();
	}

	T x2() const {
		return p2_.x();
	}

	T y2() const {
		return p2_.y();
	}

private:
	T length_;
	ParticleType p1_;
	ParticleType p2_;
};

}
