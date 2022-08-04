#pragma once
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <exception>
#include <chrono>
#include "ui_controller.hpp"
#include "earthquake_system.hpp"


namespace game {
    class GameStateController {
        public:
            static insertion_mode_t insertion_mode;
            static physics::Particle<float>* prev_joint_particle;
            static bool simulation_running;
            static EarthquakeSystem<float> earthquake_system;
            static UIController ui_controller;

            // Create empty point manager and initialize an OpenGL window
            GameStateController() {
                glfwSetErrorCallback(error_callback);
                glfwSetKeyCallback(ui_controller.window, key_callback);
                glfwSetMouseButtonCallback(ui_controller.window, mouse_button_callback);
                // Add dummy particles and joints

                // bad tower
                // earthquake_system.create_joint(50, 0, 50, 50);
                // earthquake_system.create_joint(50, 50, 50, 100);
                // earthquake_system.create_joint(50, 150, 50, 100);
                
                // earthquake_system.create_joint(100, 0, 100, 50);
                // earthquake_system.create_joint(100, 50, 100, 100);
                // earthquake_system.create_joint(100, 150, 100, 100);

                // earthquake_system.create_joint(50, 50, 100, 50);
                // earthquake_system.create_joint(50, 100, 100, 100);
                // earthquake_system.create_joint(50, 150, 100, 150);

                // // decent tower
                // earthquake_system.create_joint(250, 0, 250, 50);
                // earthquake_system.create_joint(250, 50, 250, 100);
                // earthquake_system.create_joint(250, 150, 250, 100);
                
                // earthquake_system.create_joint(300, 0, 300, 50);
                // earthquake_system.create_joint(300, 50, 300, 100);
                // earthquake_system.create_joint(300, 150, 300, 100);

                // earthquake_system.create_joint(250, 50, 300, 50);
                // earthquake_system.create_joint(250, 100, 300, 100);
                // earthquake_system.create_joint(250, 150, 300, 150);

                // earthquake_system.create_joint(250, 0, 300, 50);
                // earthquake_system.create_joint(250, 50, 300, 100);
                // earthquake_system.create_joint(250, 100, 300, 150);

                main_loop();
            }
            ~GameStateController() = default;


            static void error_callback(int error, const char* description) {
                std::cout << "GLFW Error: " << description << std::endl;
            }

            static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                    glfwSetWindowShouldClose(ui_controller.window, GL_TRUE);
            }

            static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
                if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
                    // Get cursor position
                    double xpos, ypos;
                    glfwGetCursorPos(ui_controller.window, &xpos, &ypos);
                    int x = (int)xpos;
                    int y = HEIGHT - (int)ypos; // Y starts from top, we want it from bottom

                    // Check if we're over a button
                    if (x <= ui_controller.start_bbox.right && x >= ui_controller.start_bbox.left &&
                        y <= ui_controller.start_bbox.top && y >= ui_controller.start_bbox.btm) {
                        // Start simulation
                        simulation_running = true;
                    } else if (x <= ui_controller.stop_bbox.right && x >= ui_controller.stop_bbox.left &&
                        y <= ui_controller.stop_bbox.top && y >= ui_controller.stop_bbox.btm) {
                        // Stop simulation
                        simulation_running = false;
                        insertion_mode = insertion_mode_t::PARTICLE;
                    }
                    // Over horizontal magnitude up
                    else if (x <= ui_controller.horizontal_mag_up_bbox.right && x >= ui_controller.horizontal_mag_up_bbox.left &&
                        y <= ui_controller.horizontal_mag_up_bbox.top && y >= ui_controller.horizontal_mag_up_bbox.btm) {
                        earthquake_system.inc_magnitude_x(1);
                    }
                    // Over horizontal magnitude down
                    else if (x <= ui_controller.horizontal_mag_down_bbox.right && x >= ui_controller.horizontal_mag_down_bbox.left &&
                        y >= ui_controller.horizontal_mag_down_bbox.top && y <= ui_controller.horizontal_mag_down_bbox.btm) {
                        earthquake_system.inc_magnitude_x(-1);
                    }
                    // Over vertical magnitude up
                    else if (x <= ui_controller.vertical_mag_up_bbox.right && x >= ui_controller.vertical_mag_up_bbox.left &&
                        y <= ui_controller.vertical_mag_up_bbox.top && y >= ui_controller.vertical_mag_up_bbox.btm) {
                        earthquake_system.inc_magnitude_y(1);
                    }
                    // Over vertical magnitude down
                    else if (x <= ui_controller.vertical_mag_down_bbox.right && x >= ui_controller.vertical_mag_down_bbox.left &&
                        y >= ui_controller.vertical_mag_down_bbox.top && y <= ui_controller.vertical_mag_down_bbox.btm) {
                        earthquake_system.inc_magnitude_y(-1);
                    }
                    else if (!simulation_running) {
                        // Insertion mode
                        // Snap to the nearest 20th grid point
                        if(x % 20 < 5)  x -= x % 20;
                        else            x += 20 - x % 20;
                        if(y % 20 < 5)  y -= y % 20;
                        else            y += 20 - y % 20;

                        switch(insertion_mode){
                            case insertion_mode_t::PARTICLE:
                                if (!earthquake_system.particle_at(x, y)) {
                                    prev_joint_particle = &earthquake_system.create_particle(x, y);
                                }
                                // We selected an existing particle, enter joint mode
                                else {
                                    insertion_mode = insertion_mode_t::JOINT;
                                    prev_joint_particle = earthquake_system.particle_at(x, y);
                                }
                                break;
                            case insertion_mode_t::JOINT:
                                earthquake_system.create_joint(prev_joint_particle->x(), prev_joint_particle->y(), x, y);
                                insertion_mode = insertion_mode_t::PARTICLE;
                                prev_joint_particle = nullptr;
                                break;
                            default:
                                throw std::runtime_error("Unknown insertion mode");
                                break;
                        }

                    }
                        
                }
                    
            }
            
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
                    ui_controller.render(earthquake_system.particles(), 
                                         earthquake_system.joints(), 
                                         simulation_running, // Simulation state
                                         insertion_mode,
                                         insertion_mode == insertion_mode_t::JOINT ? prev_joint_particle:nullptr, // Selected Joint
                                         earthquake_system.magnitude_x(),
                                         earthquake_system.magnitude_y());
                    glfwPollEvents();
                    
                }
            }
        
            // Called every 1/FPS seconds
            static void update_game_state(){
                // Update particles and joints
                // Calculates physics only when the simulation is running
                if (simulation_running) {
                    earthquake_system.update();
                }
            }

        };

    insertion_mode_t GameStateController::insertion_mode = insertion_mode_t::PARTICLE;
    physics::Particle<float>* GameStateController::prev_joint_particle = nullptr;
    bool GameStateController::simulation_running = false;
    UIController GameStateController::ui_controller = UIController();
    FontController UIController::font_controller = FontController();
    EarthquakeSystem<float> GameStateController::earthquake_system = EarthquakeSystem<float>(WIDTH, HEIGHT);
}