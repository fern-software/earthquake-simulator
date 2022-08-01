#pragma once

#include <utility>
#include <vector>
#include <stdexcept>

#include "particle.hpp"
#include "joint.hpp"


namespace physics {
	
template <typename T> class ParticleSystem {
public:
	using Particle = Particle<T>;
	using Joint = Joint<T>;
	using Vector = typename Particle::Vector;

	ParticleSystem(T gravity_x, T gravity_y): gravity_(Vector(gravity_x, gravity_y)){}

	// Returns the particle at the given position if it exists. Throws std::out_of_range if it 
	// does not. If two exist at the given position then it returns the first one that is found.
	// Preconditions: x, y must be representable as exact values or results are undefined.
	Particle& particle_at(T x, T y){
		for(auto& p : particles) {
			if(p.x() == x && p.y() == y) {
				return p;
			}
		}

		throw std::out_of_range("No particle found at the given position");
	}

	// Creates a new particle at the given position subject to the system's gravity and returns a
	// reference to it.
	Particle& create_particle(T x, T y) {
		particles.push_back(Particle(x, y, gravity_));
		return particles.back();
	}

	// Creates a joint in the system between two particles. If particles do not exist at the given
	// coordinates, then particles are created at them first. If particles already exist at the
	// given coordinates, new particles are not created.
	void create_joint(T x1, T y1, T x2, T y2){
		Particle& p1;
		Particle& p2;

		try {
			p1 = particle_at(x1, y1);
		} catch(std::out_of_range&){
			p1 = create_particle(x1, y1);
		}
		
		try {
			p2 = particle_at(x2, y2);
		} catch(std::out_of_range&){
			p2 = create_particle(x2, y2);
		}

		joints.push_back(Joint(p1.second, p2.second));
	}

	// TODO: implement
	void update() {
		for (auto& particle : particles) {
			particle.update(0.5f);
		}
	}

	// Returns a reference to the list of all particles in the system.
	std::vector<Particle>& particles(){
		return particles;
	}

	// Returns a reference to the list of all joints in the system.
	std::vector<Joints>& joints(){
		return joints;
	}

private:
	// constant acceleration that all particles in the system are subject to
	Vector gravity_;

	std::vector<Particle> particles;
	std::vector<Joint> joints;
};
}
