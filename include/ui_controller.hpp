#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <exception>
#include <list>
#include <CGAL/Iso_rectangle_2.h>
#include <CGAL/Point_2.h>

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

    using Bbox = CGAL::Iso_rectangle_2<CGAL::Cartesian<float>>;
    using Point = CGAL::Point_2<CGAL::Cartesian<float>>;

    // Handles all OpenGL functionality
    // Does not handle text. See FontController for that.
    class UIController {
        public:
            GLFWwindow* window;

            Bbox start_bbox;
            Bbox stop_bbox;
            Bbox horizontal_mag_up_bbox;
            Bbox horizontal_mag_down_bbox;
            Bbox vertical_mag_up_bbox;
            Bbox vertical_mag_down_bbox;

            UIController(): window(nullptr),
                            start_bbox(Point(WIDTH-55, HEIGHT-40), Point(WIDTH-40, HEIGHT-10)),
                            stop_bbox(Point(WIDTH-30, HEIGHT-38), Point(WIDTH-5, HEIGHT-12)),
                            horizontal_mag_up_bbox(Point(WIDTH-65, HEIGHT-80), Point(WIDTH-40, HEIGHT-60)),
                            horizontal_mag_down_bbox(Point(WIDTH-30, HEIGHT-60), Point(WIDTH-5, HEIGHT-80)),
                            vertical_mag_up_bbox(Point(WIDTH-65, HEIGHT-120), Point(WIDTH-40, HEIGHT-100)),
                            vertical_mag_down_bbox(Point(WIDTH-30, HEIGHT-100), Point(WIDTH-5, HEIGHT-120)) {
                try {
                    initGLFW();
                    // Load ground and sky textures
                    // Try different folders
                    try {
                        ground_texture_info = texture_utils::load_texture("../resources/brick.texture", 480, 200);
                        sky_texture_info = texture_utils::load_texture("../resources/sky.texture", 640, 480);
                    } catch(...) {
                        ground_texture_info = texture_utils::load_texture("resources/brick.texture", 480, 200);
                        sky_texture_info = texture_utils::load_texture("resources/sky.texture", 640, 480);
                    }
                    


                } catch (std::exception& e) {
                    throw std::runtime_error("Failed to initialize OpenGL");
                }
            }

            ~UIController() {
                glfwTerminate();
            }

            void render(std::list<physics::Particle<float>> particles, 
                        std::list<physics::Joint<float>> joints, 
                        bool running, 
                        insertion_mode_t insertion_mode,
                        physics::Particle<float>* selected_particle,
                        unsigned int horizontal_magnitude,
                        unsigned int vertical_magnitude,
                        float ground_height,
                        float ground_dx,
                        std::string timer) {

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);             
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0.f, WIDTH, 0.f, HEIGHT, 0.f, 1.f);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                // Draw Sky
                glColor3f(1.0f, 1.0f, 1.0f);
                texture_utils::draw_texture(0, 0, sky_texture_info, WIDTH, HEIGHT);

                // Draw a grid where the user can place particles if the simulation is not running
                if (!running){
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

                    glBegin(GL_LINES);
                    glColor4f(0.33f, 0.2f, 0.33f, 0.25f);

                    // vertical lines
                    for (int i = 0; i < WIDTH; i += 20) {
                        glVertex2f(i, 0);
                        glVertex2f(i, HEIGHT);
                    }

                    // horizontal lines
                    for (float i = ground_height; i < HEIGHT; i += 20) {
                        glVertex2f(0, i);
                        glVertex2f(WIDTH, i);
                    }
                    glEnd();

                    glBlendFunc(GL_NONE, GL_NONE);
                    glDisable(GL_BLEND);
                }

                // Print current editor mode if the simulation is not running
                glColor3f(1.f, 1.0f, 1.0f);
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

                // Draw Menu (Start, stop, timer) in the top right of the window
                if (running) {
                    glColor3f(1.0f, 1.0f, 1.0f);
                    font_controller.glPrint(WIDTH-200, HEIGHT-40, timer.c_str());
                }
                
                // Set color to green
                glColor3f(0.0f, 1.0f, 0.0f);
                // Draw start button
                glBegin(GL_TRIANGLES);
                    glVertex2f(start_bbox.xmax(), (start_bbox.ymax() + start_bbox.ymin())/2);
                    glVertex2f(start_bbox.xmin(), start_bbox.ymin());
                    glVertex2f(start_bbox.xmin(), start_bbox.ymax());
                glEnd();

                // Draw stop button
                glColor3f(1.0f, 0.0f, 0.0f);
                glBegin(GL_QUADS);
                    glVertex2f(stop_bbox.xmax(), stop_bbox.ymin());
                    glVertex2f(stop_bbox.xmax(), stop_bbox.ymax());
                    glVertex2f(stop_bbox.xmin(), stop_bbox.ymax());
                    glVertex2f(stop_bbox.xmin(), stop_bbox.ymin());
                glEnd();

                // Draw magnitude buttons
                // Horizontal adjustment
                glColor3f(1.f, 1.0f, 1.0f);
                font_controller.glPrint(WIDTH-280, HEIGHT-80, (std::string("Horiz. Shake: ") + std::to_string(horizontal_magnitude)).c_str());

                // Set color to blue
                glColor3f(0.0f, 0.0f, 1.0f);
                glBegin(GL_TRIANGLES);
                    glVertex2f(horizontal_mag_up_bbox.xmax(), horizontal_mag_up_bbox.ymin());
                    glVertex2f(horizontal_mag_up_bbox.xmin(), horizontal_mag_up_bbox.ymin());
                    glVertex2f((horizontal_mag_up_bbox.xmax() + horizontal_mag_up_bbox.xmin()) / 2, horizontal_mag_up_bbox.ymax());
                glEnd();
                glBegin(GL_TRIANGLES);
                    glVertex2f(horizontal_mag_down_bbox.xmax(), horizontal_mag_down_bbox.ymax());
                    glVertex2f(horizontal_mag_down_bbox.xmin(), horizontal_mag_down_bbox.ymax());
                    glVertex2f((horizontal_mag_down_bbox.xmax() + horizontal_mag_down_bbox.xmin()) / 2, horizontal_mag_down_bbox.ymin());
                glEnd();

                // Vertical adjustment
                glColor3f(1.f, 1.0f, 1.0f);
                font_controller.glPrint(WIDTH-267, HEIGHT-120, (std::string("Vert. Shake: ") + std::to_string(vertical_magnitude)).c_str());

                // Set color to blue
                glColor3f(0.0f, 0.0f, 1.0f);
                glBegin(GL_TRIANGLES);
                    glVertex2f(vertical_mag_up_bbox.xmax(), vertical_mag_up_bbox.ymin());
                    glVertex2f(vertical_mag_up_bbox.xmin(), vertical_mag_up_bbox.ymin());
                    glVertex2f((vertical_mag_up_bbox.xmax() + vertical_mag_up_bbox.xmin()) / 2, vertical_mag_up_bbox.ymax());
                glEnd();
                glBegin(GL_TRIANGLES);
                    glVertex2f(vertical_mag_down_bbox.xmax(), vertical_mag_down_bbox.ymax());
                    glVertex2f(vertical_mag_down_bbox.xmin(), vertical_mag_down_bbox.ymax());
                    glVertex2f((vertical_mag_down_bbox.xmax() + vertical_mag_down_bbox.xmin()) / 2, vertical_mag_down_bbox.ymin());
                glEnd();

                // Draw ground
                glColor3f(1.0f, 1.0f, 1.0f);
                texture_utils::draw_texture(0, 0, ground_texture_info, WIDTH + ground_dx + 100, ground_height);

                // Draw particles
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glEnable(GL_POINT_SMOOTH);
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
                window = glfwCreateWindow(WIDTH, HEIGHT, "Earthquake Simulator", NULL, NULL);
                glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
                if (!window) {
                    glfwTerminate();
                    throw std::runtime_error("Failed to create window");
                }

                glfwMakeContextCurrent(window);
                glViewport(0, 0, WIDTH, HEIGHT);
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                glDisable(GL_DEPTH_TEST);
            }

            // Should close the window?
            bool shouldClose() const {
                return glfwWindowShouldClose(window);
            }

        private:
            static FontController font_controller;
            texture_utils::texture_info_t ground_texture_info;
            texture_utils::texture_info_t sky_texture_info;
            
    };
}