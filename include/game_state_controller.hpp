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
#include <list>
#include "font_controller.hpp"
#include "earthquake_system.hpp"


namespace game {
    #define PIXEL_FORMAT GL_RGB
    #define WIDTH 640
    #define HEIGHT 480
    #define FPS 60

    enum class insertion_mode_t {
        PARTICLE,
        JOINT
    };

    // Handles all OpenGL functionality
    // Does not handle text, it's fucked
    class UIController {
        public:
            GLFWwindow* window;

            struct Bbox {
                int left;
                int btm;
                int right;
                int top;
            };
            Bbox start_bbox = {WIDTH-55, HEIGHT-40,
                               WIDTH-40, HEIGHT-10};

            Bbox stop_bbox = {WIDTH-30, HEIGHT-38,
                              WIDTH-5, HEIGHT-12};

            UIController(): window(nullptr) {
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

            void render(std::list<physics::Particle<float>> particles, 
                        std::list <physics::Joint<float>> joints, 
                        bool running, 
                        insertion_mode_t insertion_mode,
                        physics::Particle<float>* selected_particle) {

                glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
                glDisable (GL_DEPTH_TEST);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                glViewport(0, 0, width, height);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0.f, width, 0.f, height, 0.f, 1.f);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                // Print current editor mode if the simulation is not running
                if (!running) {
                    switch(insertion_mode) {
                        case insertion_mode_t::PARTICLE:
                            font_controller.glPrint(10, HEIGHT-30, "Inserting Particles");
                            break;
                        case insertion_mode_t::JOINT:
                            font_controller.glPrint(10, HEIGHT-30, "Inserting Joints");
                            break;
                    }
                    font_controller.glPrint(WIDTH-180, HEIGHT-30, "Paused");
                }
                else {
                    font_controller.glPrint(WIDTH-180, HEIGHT-30, "Running");
                }

                // Draw Menu (Start and stop buttons) in the top right of the window

                // Set color to green
                glColor3f(0.0f, 1.0f, 0.0f);
                // Draw start button (small 45 degree rotated triangle)
                glBegin(GL_TRIANGLES);
                    glVertex2f(start_bbox.right, (start_bbox.top + start_bbox.btm)/2);
                    glVertex2f(start_bbox.left, start_bbox.btm);
                    glVertex2f(start_bbox.left, start_bbox.top);
                glEnd();

                // Draw stop button (Small red square to the right of the start button)
                glColor3f(1.0f, 0.0f, 0.0f);
                glBegin(GL_QUADS);
                    glVertex2f(stop_bbox.right, stop_bbox.btm);
                    glVertex2f(stop_bbox.right, stop_bbox.top);
                    glVertex2f(stop_bbox.left, stop_bbox.top);
                    glVertex2f(stop_bbox.left, stop_bbox.btm);
                glEnd();

                // Draw particles
                glEnable(GL_ALPHA_TEST);
                glAlphaFunc(GL_NOTEQUAL, 0);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glEnable( GL_POINT_SMOOTH );
                glPointSize(8.0);

                glBegin(GL_POINTS);
                for (auto& particle : particles) {
                    if(selected_particle && particle == *selected_particle) {
                        glColor3f(0.0f, 1.0f, 0.0f);
                    }
                    else {
                        glColor3f(1.0f, 0.0f, 0.0f);
                    }
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
            }

            // Should close the window?
            bool shouldClose() const {
                return glfwWindowShouldClose(window);
            }

        private:
            static FontController font_controller;
            
    };

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
                                         insertion_mode == insertion_mode_t::JOINT ? prev_joint_particle:nullptr); // Insertion mode
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