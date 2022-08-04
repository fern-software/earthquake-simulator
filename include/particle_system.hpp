#pragma once

#include <utility>
#include <list>
#include <stdexcept>

#include "particle.hpp"
#include "joint.hpp"

namespace physics {

// Represents a system of Particles and Joints within a bounded box subject to constant gravity.
template <typename T> class ParticleSystem {
public:
	using Point = typename Particle<T>::Point;
	using Vector = typename Particle<T>::Vector;
	using Rectangle = typename Particle<T>::Rectangle;

	ParticleSystem(
		T lower_bound_x,
		T upper_bound_x,
		T lower_bound_y,
		T upper_bound_y,
		T gravity_x,
		T gravity_y
	) :
		bounding_box_(Rectangle(Point(lower_bound_x, lower_bound_y), Point(upper_bound_x, upper_bound_y))),
		gravity_(Vector(gravity_x, gravity_y))
	{}

	// Returns the particle at the given position if it exists. Returns nullptr if it does not.
	// If two exist at the given position then it returns the first one that is found.
	// Preconditions: x, y must be representable as exact values or results are undefined.
	Particle<T>* particle_at(T x, T y){
		for(auto& p : particles_) {
			if(p.x() == x && p.y() == y) {
				return &p;
			}
		}

		return nullptr;
	}

	// Creates a new particle at the given position subject to the system's gravity and returns a
	// reference to it.
	Particle<T>& create_particle(T x, T y, bool fixed){
		particles_.push_back(Particle<T>(x, y, fixed, bounding_box_, gravity_));
		return particles_.back();
	}

	// Creates a joint in the system between the two given particles. Retruns a reference to the
	// joint created.
	Joint<T>& create_joint(Particle<T>& p1, Particle<T>& p2){
		joints_.push_back(Joint(p1, p2));
		return joints_.back();
	}

	// Updates the simulation by a given timestep dt.
	void update(T dt){
		// updates positions of all particles as effected by gravity
		for(auto& particle : particles_){
			particle.update(dt);
		}

		// relaxation loop
		// the number of iterations used partly determines the accuracy of the simulation
		// less iterations results in the joints behaving less like rigid bodies and more like
		// springs
		for(int i = 0; i < 10; ++i){
			for(auto& joint : joints_){
				joint.maintain_length();
			}

			for(auto& particle : particles_){
				particle.stay_in_bounds();
			}
		}
	}

	// Returns a reference to the list of all particles in the system.
	std::list<Particle<T>>& particles(){
		return particles_;
	}

	// Returns a reference to the list of all joints in the system.
	std::list<Joint<T>>& joints(){
		return joints_;
	}

	// Moves the lower bound of the system.
	void move_lower_bound(T dx, T dy){
		bounding_box_ = Rectangle(bounding_box_.min() + Vector(dx, dy), bounding_box_.max());
	}

	// Moves the upper bound of the system.
	void move_upper_bound(T dx, T dy){
		bounding_box_ = Rectangle(bounding_box_.min(), bounding_box_.max() + Vector(dx, dy));
	}

	// Returns the bounding box of the system.
	Rectangle& bounding_box(){
		return bounding_box_;
	}

private:
	// Actual bounding box of the system, all particles must stay within this box.
	Rectangle bounding_box_;
	
	// Constant acceleration that all particles in the system are subject to.
	Vector gravity_;

	// Lists of all particles and joints in the system.
	// It is important that the particles and joints are stored in lists instead of vectors because
	// std::list guarantees that references to elements are valid as long as the element that the
	// reference points to is in the list (ie: the elements has not been erased from the list). 
	// This program uses references to elements in these lists extensively, so even though there is
	// a small performance hit iterating through lists compared to vectors, it is neccessary.
	std::list<Particle<T>> particles_;
	std::list<Joint<T>> joints_;
};

}
