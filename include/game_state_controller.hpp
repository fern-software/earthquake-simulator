#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <exception>
#include <chrono>
#include <vector>
#include "particle_joint_manager.hpp"
#include "particle.hpp"
#include "joint.hpp"

#define PIXEL_FORMAT GL_RGB
#define WIDTH 640
#define HEIGHT 480
#define FPS 60

namespace game {
    class UIController {
        public:
            UIController(int argc, char* argv[]): window(nullptr), ratio(0.0f) {
                try {
                    initGLFW();
                } catch (std::exception& e) {
                    std::cout << "Failed to initialize OpenGL" << std::endl;
                    exit(1);
                }
            }

            ~UIController() {
                glfwTerminate();
            }

            static void error_callback(int error, const char* description) {
                std::cout << "GLFW Error: " << description << std::endl;
            }
            static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                    glfwSetWindowShouldClose(window, GL_TRUE);
            }

            void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
                if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
                    
                }
                    
            }

            void render(std::vector<physics::Particle<float>> particles, std::vector <physics::Joint<float>> joints) {
                glClear(GL_COLOR_BUFFER_BIT);
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                ratio = width / (float)height;
                glViewport(0, 0, width, height);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                // glMatrixMode(GL_MODELVIEW);
                // glLoadIdentity();
                glOrtho(0.0f, width, 0.f, height, 0.f, 1.f);

                // Draw particles
                glEnable(GL_ALPHA_TEST);
                glAlphaFunc(GL_NOTEQUAL, 0);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glEnable( GL_POINT_SMOOTH );
                glPointSize( 8.0 );

                glBegin(GL_POINTS);
                for (auto& particle : particles) {
                    glColor3f(1.0f, 0.0f, 0.0f);
                    glVertex2f(particle.x(), particle.y());
                }
                glEnd();
                glDisable(GL_POINT_SMOOTH);
                glBlendFunc(GL_NONE, GL_NONE);
                glDisable(GL_BLEND);

                // Draw joints
                glBegin(GL_LINES);
                for (auto& joint : joints) {
                    glColor3f(0.0f, 0.0f, 1.0f);
                    glVertex2f(joint.x1(), joint.y1());
                    glVertex2f(joint.x2(), joint.y2());
                }
                glEnd();
                glfwSwapBuffers(window);
            }

            void initGLFW() {
                glfwSetErrorCallback(error_callback);
                if (!glfwInit())
                    throw std::runtime_error("Failed to initialize GLFW");
                
                int major, minor, rev;
                glfwGetVersion(&major, &minor, &rev);
                std::cout << "glfw major.minor " << major << "." << minor << "." << rev << std::endl;

                window = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);
                if (!window) {
                    glfwTerminate();
                    throw std::runtime_error("Failed to create window");
                }

                glfwMakeContextCurrent(window);
                glfwSetKeyCallback(window, key_callback);
                glfwSetMouseButtonCallback(window, mouse_button_callback);
            }

            // Should close the window?
            bool shouldClose() {
                return glfwWindowShouldClose(window);
            }

        private:
            GLFWwindow* window;
            float ratio;
    };

    class GameStateController {
        public:
            // Create empty point manager and initialize openGL window
            GameStateController(int argc, char* argv[]): point_manager(ParticleJointManager<float>()), 
                                                         ui_controller(UIController(argc, argv)),
                                                         simulation_running(false) {
                // Add dummy particles and joints
                point_manager.addParticle(physics::Particle<float>(
                    50, 50, // Position
                    1, 2 // Acceleration
                ));
                point_manager.addParticle(physics::Particle<float>(
                    100, 200, // Position
                    1, 2 // Acceleration
                ));
                point_manager.addJoint(physics::Joint<float>(
                    point_manager.getParticles()[0],
                    point_manager.getParticles()[1]
                ));

                main_loop();
            }
            ~GameStateController() = default;
            
        private:

            // Calls update_game_state every 1/FPS seconds
            // Calls render as often as possible
            void main_loop() {
                auto a = std::chrono::high_resolution_clock::now();
               while (!ui_controller.shouldClose()) {
                    auto b = std::chrono::high_resolution_clock::now();
                    auto delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count();
                    if(delta_time >= 1000 / FPS) {
                        a = b;
                        update_game_state();
                    }
                    ui_controller.render(point_manager.getParticles(), point_manager.getJoints());
                    glfwPollEvents();
                    
                }
            }
        
            // Called every 1/FPS seconds
            void update_game_state(){
                // Update particles and joints
                // Calculates physics only when the simulation is running
                if (simulation_running) {
                    point_manager.update();
                }
            }

            bool simulation_running;
            ParticleJointManager<float> point_manager;
            UIController ui_controller;
    };
}