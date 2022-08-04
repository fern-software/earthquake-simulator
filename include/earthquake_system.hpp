#pragma once

#include <cmath>
#include <list>

#include "particle_system.hpp"
#include "particle.hpp"
#include "joint.hpp"

namespace game {

// Represents a ParticleSystem specific to an earthquake simulation.
template <typename T> class EarthquakeSystem {
public:
	// Creates a new EarthquakeSystem with the given width, height, *realistic* gravity and an
	// inital ground level which particles position's may not go below.
	EarthquakeSystem(unsigned int width, unsigned int height, int init_ground_level = 40) :
		run_time_(0),
		ground_dx_(0),
		system_(physics::ParticleSystem<T>(0, width, init_ground_level, height, 0, -1)){}

	physics::Particle<T>* particle_at(T x, T y){
		return system_.particle_at(x, y);
	}

	// Creates a particle in the system. If the particle is on the ground (ie: y = 0), then it is
	// fixed. Returns a reference to the particle created. If the particle is already in the system
	// then it is not added again and reference to that particle is returned.
	physics::Particle<T>& create_particle(T x, T y){
		if(!particle_at(x, y)){
			return system_.create_particle(x, y, y == 40 ? true : false);
		}
		return *particle_at(x, y);
	}

	// Creates a joint in the system between two particles. If particles do not exist at the given
	// coordinates, then particles are created at them first. If particles already exist at the
	// given coordinates, new particles are not created.
	void create_joint(T x1, T y1, T x2, T y2){
		physics::Particle<T>* p1 = system_.particle_at(x1, y1);
		if(!p1){
			p1 = &create_particle(x1, y1);
		}
		
		physics::Particle<T>* p2 = system_.particle_at(x2, y2);
		if(!p2){
			p2 = &create_particle(x2, y2);
		}

		system_.create_joint(*p1, *p2);
	}

	// Updates the simulation by one timestep.
	void update(){
		run_time_ += 0.25;

		shake_ground();
		system_.update(0.1);
	}

	// Moves the particles touching the ground a set amount depending on the system's run time.
	// This creates a shaking effect over subsequent calls. Also moves the ground up and down if
	// the vertical magnitude of the earthquake is greater than 0.
	void shake_ground(){
		T dx = std::sin(run_time_ * 1.3); // TODO: make this change depending on horizontal magnitude
		T dy = std::sin(run_time_ * 5); // TODO: make this change depending on vertical magnitude

		// update bounding box of system
		system_.move_lower_bound(0, dy);

		// update ground position
		ground_dx_ += dx;

		// Move particles touching the ground.
		// Do to floating point inaccuracies, we need to update fixed particles differently.
		for(auto& particle : system_.particles()){
			if(particle.fixed()){
				particle.set_position(particle.x() + dx, system_.bounding_box().ymin());
			}
			else if(particle.y() <= system_.bounding_box().ymin()){
				particle.move(dx, dy);
			}
		}
	}

	// Returns a reference to the list of all particles in the system.
	std::list<physics::Particle<T>>& particles(){
		return system_.particles();
	}

	// Returns a reference to the list of all joints in the system.
	std::list<physics::Joint<T>>& joints(){
		return system_.joints();
	}

	// Returns the current height of the ground.
	T ground_height(){
		return system_.bounding_box().ymin();
	}

	T ground_dx(){
		return ground_dx_;
	}

private:
	// total time system has been running
	T run_time_;

	// the amount to the left of the right the ground has moved
	T ground_dx_;

	// underlying particle system
	physics::ParticleSystem<T> system_;
};

};
