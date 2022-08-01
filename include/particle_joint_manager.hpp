#pragma once
#include "particle.hpp"
#include "joint.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/named_function_params.hpp>
#include <boost/graph/visitors.hpp>
#include <vector>


namespace game {
    template <typename T>
    class ParticleJointManager {
        public:
            ParticleJointManager(): particles({}), joints({}) {}

            ~ParticleJointManager() = default;

            // Updates the position of all points in the simulation
            void update() {
                for (auto& particle : particles) {
                    particle.update(0.5f);
                }
                // TODO: Joints
            }

            // Adds a particle to the simulation
            void addParticle(physics::Particle<T> particle) {
                particles.push_back(particle);
            }

            // Adds a joint to the simulation
            void addJoint(physics::Joint<T> joint) {
                joints.push_back(joint);
            }

            // Get particles
            std::vector<physics::Particle<T>>& getParticles() {
                return particles;
            }

            // Get joints
            std::vector<physics::Joint<T>>& getJoints() {
                return joints;
            }
        private:
            std::vector<physics::Particle<T>> particles;
            std::vector<physics::Joint<T>> joints;
    };
}