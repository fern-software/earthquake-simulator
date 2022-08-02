#pragma once

#include <utility>
#include <vector>
#include <stdexcept>

#include "particle.hpp"
#include "joint.hpp"


namespace physics {
	
template <typename T> class ParticleSystem {
public:
	using Vector = typename Particle<T>::Vector;

	ParticleSystem(unsigned int width, unsigned int height, T gravity_x, T gravity_y): width_(width), height_(height), gravity_(Vector(gravity_x, gravity_y)){}

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
	Particle<T>& create_particle(T x, T y) {
		particles_.push_back(Particle<T>(x, y, width_, height_, gravity_));
		return particles_.back();
	}

	// Creates a joint in the system between two particles. If particles do not exist at the given
	// coordinates, then particles are created at them first. If particles already exist at the
	// given coordinates, new particles are not created.
	void create_joint(T x1, T y1, T x2, T y2){
		Particle<T>* p1 = particle_at(x1, y1);
		if(!p1){
			p1 = &create_particle(x1, y1);
		}
		
		Particle<T>* p2 = particle_at(x2, y2);
		if(!p2){
			p2 = &create_particle(x2, y2);
		}

		joints_.push_back(Joint(*p1, *p2));
	}

	// TODO: implement
	void update() {
		// updates positions of all particles as effected by gravity
		for(auto& particle : particles_){
			particle.update(0.5f);
		}

		// relaxation loop
		// the number of iterations used partly determines the accuracy of the simulation
		// less iterations results in the joints behaving less like rigid bodies
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
	std::vector<Particle<T>>& particles(){
		return particles_;
	}

	// Returns a reference to the list of all joints in the system.
	std::vector<Joint<T>>& joints(){
		return joints_;
	}

private:
	// width and height of the system
	unsigned int width_, height_;
	
	// constant acceleration that all particles in the system are subject to
	Vector gravity_;

	// list of particles and joints in the system
	std::vector<Particle<T>> particles_;
	std::vector<Joint<T>> joints_;
};
}
