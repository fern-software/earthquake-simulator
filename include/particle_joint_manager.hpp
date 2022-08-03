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
            ParticleJointManager(): particles({}), joints({}) {
                // Reallocating particles breaks joint references to them
                particles.reserve(100);
                joints.reserve(100);
            }

            ~ParticleJointManager() = default;

            // Updates the position of all points in the simulation
            void update() {
                for (auto& particle : particles) {
                    particle.update(0.5f);
                }
                // TODO: Joints
            }

            // Find particle at a given position
            // If this is too slow use a hashtable or something
            physics::Particle<T>* particleAt(const double x, const double y) {
                for (auto& particle : particles) {
                    if (particle.x() == x && particle.y() == y) {
                        return &particle;
                    }
                }
                return nullptr;
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