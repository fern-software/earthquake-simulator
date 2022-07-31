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

            void render(std::vector<physics::Particle<float>> particles, std::vector <physics::Joint<float>> joints) {
                glClear(GL_COLOR_BUFFER_BIT);
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                ratio = width / (float)height;
                glViewport(0, 0, width, height);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
                glBegin(GL_TRIANGLES);
                glColor3f(1.f, 0.f, 0.f);
                glVertex3f(-0.6f, -0.4f, 0.f);
                glColor3f(0.f, 1.f, 0.f);
                glVertex3f(0.6f, -0.4f, 0.f);
                glColor3f(0.f, 0.f, 1.f);
                glVertex3f(0.f, 0.6f, 0.f);
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

                window = glfwCreateWindow(HEIGHT, WIDTH, "Hello World", NULL, NULL);
                if (!window) {
                    glfwTerminate();
                    throw std::runtime_error("Failed to create window");
                }

                glfwMakeContextCurrent(window);
                glfwSetKeyCallback(window, key_callback);
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
            GameStateController(int argc, char* argv[]): point_manager(ParticleJointManager<float>()), ui_controller(UIController(argc, argv)) {
                // Add dummy particles and joints
                point_manager.addParticle(physics::Particle<float>(
                    physics::Particle<float>::Point(5, 5)
                ));
                point_manager.addParticle(physics::Particle<float>(
                    physics::Particle<float>::Point(10, 10)
                ));
                point_manager.addJoint(physics::Joint<float>(
                    physics::Particle<float>::Point(5, 5),
                    physics::Joint<float>::Point(10, 10)
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
        
            // Called every frame
            static void update_game_state(){}

            ParticleJointManager<float> point_manager;
            UIController ui_controller;
    };
}