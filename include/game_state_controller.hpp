#pragma once
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <exception>
#include <chrono>
#include "ui_controller.hpp"
#include <list>
#include "earthquake_system.hpp"


namespace game {
    class GameStateController {
        using Particle = physics::Particle<float>;
        public:
            static insertion_mode_t insertion_mode;
            static Particle* prev_joint_particle;
            static bool simulation_running;
            static EarthquakeSystem<float> earthquake_system;
            static UIController ui_controller;

            // Create empty point manager and initialize an OpenGL window
            GameStateController() {
                glfwSetErrorCallback(error_callback);
                glfwSetKeyCallback(ui_controller.window, key_callback);
                glfwSetMouseButtonCallback(ui_controller.window, mouse_button_callback);

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

                    Point pos(x, y);

                    // Check if we're over a button
                    if (ui_controller.start_bbox.has_on_bounded_side(pos)) {
                        // Start simulation
                        simulation_running = true;
                    } else if (ui_controller.stop_bbox.has_on_bounded_side(pos)) {
                        // Stop simulation
                        simulation_running = false;
                        insertion_mode = insertion_mode_t::PARTICLE;
                    }
                    // Over horizontal magnitude up
                    else if (ui_controller.horizontal_mag_up_bbox.has_on_bounded_side(pos)) {
                        earthquake_system.inc_magnitude_x(1);
                    }
                    // Over horizontal magnitude down
                    else if (ui_controller.horizontal_mag_down_bbox.has_on_bounded_side(pos)) {
                        earthquake_system.inc_magnitude_x(-1);
                    }
                    // Over vertical magnitude up
                    else if (ui_controller.vertical_mag_up_bbox.has_on_bounded_side(pos)) {
                        earthquake_system.inc_magnitude_y(1);
                    }
                    // Over vertical magnitude down
                    else if (ui_controller.vertical_mag_down_bbox.has_on_bounded_side(pos)) {
                        earthquake_system.inc_magnitude_y(-1);
                    }
                    else if (!simulation_running) {
                        // Insertion mode
                        Particle* p = earthquake_system.particle_near(x, y, 10);

                        // Snap to the nearest 20x20 grid point from the ground up
                        int y_snap = static_cast<int>(earthquake_system.ground_height()) - INIT_GROUND_LEVEL;
                        if(x % 20 < 10)             x -= x % 20;
                        else                        x += 20 - x % 20;
                        if((y - y_snap) % 20 < 10)  y -= (y - y_snap) % 20;
                        else                        y += 20 - (y - y_snap) % 20;

                        switch(insertion_mode){
                            case insertion_mode_t::PARTICLE:
                                if (!p) {
                                    prev_joint_particle = &earthquake_system.create_particle(x, y);
                                }
                                // We selected an existing particle, enter joint mode
                                else {
                                    insertion_mode = insertion_mode_t::JOINT;
                                    prev_joint_particle = p;
                                }
                                break;
                            case insertion_mode_t::JOINT:
                                if (p) {
                                    earthquake_system.create_joint(prev_joint_particle->x(), prev_joint_particle->y(), p->x(), p->y());
                                }
                                else {
                                    earthquake_system.create_joint(prev_joint_particle->x(), prev_joint_particle->y(), x, y);
                                }
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
            constexpr static long update_rate = 1000 / FPS;

            // Calls update_game_state every 1/FPS seconds
            // Calls render as often as possible
            void main_loop() {
                auto start_time = std::chrono::high_resolution_clock::now();
                auto a = start_time;
                auto time_running = start_time;

                while (!ui_controller.shouldClose()) {
                    auto b = std::chrono::high_resolution_clock::now();
                    auto delta_time = std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count();
                    if(delta_time >= update_rate) {
                        a = b;
                        update_game_state();
                    }
                    // If simulation is running tick the timer
                    if (simulation_running) {
                        time_running += std::chrono::milliseconds(delta_time);
                    }
                    // If simulation state goes from stopped to running, invalidate the selected joint particle
                    if (prev_joint_particle && simulation_running) {
                        prev_joint_particle = nullptr;
                    }

                    ui_controller.render(earthquake_system.particles(), 
                                         earthquake_system.joints(), 
                                         simulation_running, // Simulation state
                                         insertion_mode,
                                         insertion_mode == insertion_mode_t::JOINT ? prev_joint_particle : nullptr, // Selected Joint
                                         earthquake_system.magnitude_x(), // Horizontal shake
                                         earthquake_system.magnitude_y(), // Vertical shake
                                         earthquake_system.ground_height(), // Ground height
                                         earthquake_system.ground_dx(), // Ground dx
                                         std::string("Time: ") + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(time_running - start_time).count() / 1000) + "s" // Timer string
                                         ); 

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
    EarthquakeSystem<float> GameStateController::earthquake_system = EarthquakeSystem<float>(WIDTH, HEIGHT, INIT_GROUND_LEVEL);
}