#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <exception>
#include <list>

#include "font_controller.hpp"
#include "particle.hpp"
#include "joint.hpp"

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
    // Does not handle text. See FontController for that.
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

            Bbox horizontal_mag_up_bbox = {WIDTH-65, HEIGHT-80,
                                           WIDTH-40, HEIGHT-60};

            Bbox horizontal_mag_down_bbox = {WIDTH-30, HEIGHT-60,
                                             WIDTH-5, HEIGHT-80};

            Bbox vertical_mag_up_bbox = {WIDTH-65, HEIGHT-120,
                                         WIDTH-40, HEIGHT-100};

            Bbox vertical_mag_down_bbox = {WIDTH-30, HEIGHT-100,
                                           WIDTH-5, HEIGHT-120};

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
                        physics::Particle<float>* selected_particle,
                        unsigned int horizontal_magnitude,
                        unsigned int vertical_magnitude) {

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
                    font_controller.glPrint(WIDTH-180, HEIGHT-40, "Paused");
                }
                else {
                    font_controller.glPrint(WIDTH-180, HEIGHT-40, "Running");
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

                // Draw magnitude buttons
                // Horizontal adjustment
                font_controller.glPrint(WIDTH-280, HEIGHT-80, (std::string("Horiz. Shake: ") + std::to_string(horizontal_magnitude)).c_str());

                // Set color to blue
                glColor3f(0.0f, 0.0f, 1.0f);
                glBegin(GL_TRIANGLES);
                    glVertex2f(horizontal_mag_up_bbox.right, horizontal_mag_up_bbox.btm);
                    glVertex2f(horizontal_mag_up_bbox.left, horizontal_mag_up_bbox.btm);
                    glVertex2f((horizontal_mag_up_bbox.right + horizontal_mag_up_bbox.left) / 2, horizontal_mag_up_bbox.top);
                glEnd();
                glBegin(GL_TRIANGLES);
                    glVertex2f(horizontal_mag_down_bbox.right, horizontal_mag_down_bbox.btm);
                    glVertex2f(horizontal_mag_down_bbox.left, horizontal_mag_down_bbox.btm);
                    glVertex2f((horizontal_mag_down_bbox.right + horizontal_mag_down_bbox.left) / 2, horizontal_mag_down_bbox.top);
                glEnd();

                // Vertical adjustment
                font_controller.glPrint(WIDTH-267, HEIGHT-120, (std::string("Vert. Shake: ") + std::to_string(vertical_magnitude)).c_str());

                // Set color to blue
                glColor3f(0.0f, 0.0f, 1.0f);
                glBegin(GL_TRIANGLES);
                    glVertex2f(vertical_mag_up_bbox.right, vertical_mag_up_bbox.btm);
                    glVertex2f(vertical_mag_up_bbox.left, vertical_mag_up_bbox.btm);
                    glVertex2f((vertical_mag_up_bbox.right + vertical_mag_up_bbox.left) / 2, vertical_mag_up_bbox.top);
                glEnd();
                glBegin(GL_TRIANGLES);
                    glVertex2f(vertical_mag_down_bbox.right, vertical_mag_down_bbox.btm);
                    glVertex2f(vertical_mag_down_bbox.left, vertical_mag_down_bbox.btm);
                    glVertex2f((vertical_mag_down_bbox.right + vertical_mag_down_bbox.left) / 2, vertical_mag_down_bbox.top);
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
}