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
#include <utility>
#include <pango/pangocairo.h>
#include "particle_joint_manager.hpp"
#include "particle.hpp"
#include "joint.hpp"

#define PIXEL_FORMAT GL_RGB
#define WIDTH 640
#define HEIGHT 480
#define FPS 60
#define FONT "Sans Bold 18"

        
namespace game {
    enum class insertion_mode_t {
        PARTICLE,
        JOINT
    };

    // Handles all OpenGL functionality and user input
    class UIController {
        public:
            GLFWwindow* window;
            struct texture_info_t {
                int id;
                int width;
                int height;
            };

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

            // Creates a texture and returns its OpenGL ID
            unsigned int create_texture (unsigned int width, unsigned int height, unsigned char *pixels) {
                unsigned int texture_id;

                glGenTextures (1, &texture_id);
                glBindTexture (GL_TEXTURE_2D, texture_id);
                glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexImage2D (GL_TEXTURE_2D,
                                0,
                                GL_RGBA,
                                width,
                                height,
                                0,
                                GL_BGRA,
                                GL_UNSIGNED_BYTE,
                                pixels);

                return texture_id;
            }

            void draw_texture (int x, int y, int width, int height, unsigned int texture_id) {
                /* Render a texture in immediate mode. */
                int x_top = x + width;
                int y_top = y + height;
                glEnable (GL_BLEND);
                glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glEnable (GL_TEXTURE_2D);
                glClear(GL_COLOR_BUFFER_BIT);
                glPushMatrix();
                glBindTexture (GL_TEXTURE_2D, texture_id);
                glColor3f (1.f, 1.0f, 1.0f);

                glBegin(GL_QUADS);
                    glTexCoord2i(0, 0); glVertex2i(x, y_top);
                    glTexCoord2i(0, 1); glVertex2i(x, y);
                    glTexCoord2i(1, 1); glVertex2i(x_top, y);
                    glTexCoord2i(1, 0); glVertex2i(x_top, y_top);
                glEnd();
                glDisable(GL_TEXTURE_2D);
                glDisable(GL_BLEND);
                glPopMatrix();
            }

            cairo_t* create_cairo_context(int width, int height, int channels, cairo_surface_t** surf, unsigned char** buffer) {
                *buffer = (unsigned char*)calloc(channels * width * height, sizeof (unsigned char));
                *surf = cairo_image_surface_create_for_data (*buffer,
                                                            CAIRO_FORMAT_ARGB32,
                                                            width,
                                                            height,
                                                            channels * width);
                return cairo_create (*surf);
            }

            cairo_t* create_layout_context () {
                cairo_surface_t *temp_surface;
                cairo_t *context;

                temp_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 0, 0);
                context = cairo_create (temp_surface);
                cairo_surface_destroy (temp_surface);

                return context;
            }

            void get_text_size(PangoLayout *layout, int *width, int *height) {
                pango_layout_get_size (layout, width, height);
                /* Divide by pango scale to get dimensions in pixels. */
                *width /= PANGO_SCALE;
                *height /= PANGO_SCALE;
            }

            // Handles everything needed to turn text into a texture
            // Returns the new texture ID, text width, and text height
            texture_info_t render_text (const char *text) {
                int texture_id;
                int text_width;
                int text_height;
                unsigned char* surface_data = NULL;

                cairo_t *layout_context;
                cairo_t *render_context;
                cairo_surface_t *surface;
                
                PangoFontDescription *desc;
                PangoLayout *layout;

                layout_context = create_layout_context();

                /* Create a PangoLayout, set the font and text */
                layout = pango_cairo_create_layout(layout_context);
                pango_layout_set_text(layout, text, -1);

                /* Load the font */
                desc = pango_font_description_from_string(FONT);
                pango_layout_set_font_description(layout, desc);
                pango_font_description_free(desc);

                /* Get text dimensions and create a context to render to */
                get_text_size(layout, &text_width, &text_height);
                render_context = create_cairo_context(text_width, text_height, 4, &surface, &surface_data);

                /* Render */
                cairo_set_source_rgba (render_context, 1, 1, 1, 1);
                pango_cairo_show_layout (render_context, layout);
                texture_id = create_texture(text_width, text_height, surface_data);

                /* Clean up */
                free(surface_data);
                g_object_unref(layout);
                cairo_destroy(layout_context);
                cairo_destroy(render_context);
                cairo_surface_destroy(surface);

                return {texture_id, text_width, text_height};
            }

            void glPrint(const int x, const int y, const char *text) {
                texture_info_t info;
                if (textures.find(text) == textures.end()) {
                    info = render_text(text);
                    textures[text] = info;
                }
                else {
                    info = textures[text];
                }
                draw_texture(x, y, info.width, info.height, info.id);
                
                // int texture_id;
                // int text_width = 0;
                // int text_height = 0;
                // render_text(text, &texture_id, &text_width, &text_height);
                

                // draw_texture(x, y, textures[it]., text_width, text_height);
            }

            void render(std::vector<physics::Particle<float>> particles, std::vector <physics::Joint<float>> joints) {
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
                glPrint(10, 10, "Hello World");
                
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
            std::map<std::string, texture_info_t> textures;
    };

    class GameStateController {
        public:
            
            static insertion_mode_t insertion_mode;
            static physics::Particle<float>* prev_joint_particle;
            static bool simulation_running;
            static ParticleJointManager<float> point_manager;
            static UIController ui_controller;

            // Create empty point manager and initialize an OpenGL window
            GameStateController() {
                glfwSetErrorCallback(error_callback);
                glfwSetKeyCallback(ui_controller.window, key_callback);
                glfwSetMouseButtonCallback(ui_controller.window, mouse_button_callback);

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
                    } else if (!simulation_running) {
                        // Insertion mode
                        // Snap to the nearest 20th grid point
                        if(x % 20 < 5)  x -= x % 20;
                        else            x += 20 - x % 20;
                        if(y % 20 < 5)  y -= y % 20;
                        else            y += 20 - y % 20;

                        if (insertion_mode == insertion_mode_t::PARTICLE){
                            // If the mouse is over nothing and the insertion mode is particle, insert a particle
                            if (!point_manager.particleAt(x, y)) {
                                point_manager.addParticle(physics::Particle<float>(x, y));
                            }

                            // We selected an existing particle, enter joint mode
                            else {
                                insertion_mode = insertion_mode_t::JOINT;
                                prev_joint_particle = point_manager.particleAt(x, y);
                            }
                            
                        }
                        else if (insertion_mode == insertion_mode_t::JOINT){
                            // If the insertion mode is joint, insert a joint (Joint handles collision detection)
                            // point_manager.addJoint(physics::Joint<float>(x, y));
                        }
                    }
                    
                }
                    
            }
            
        private:

            // Calls update_game_state every 1/FPS seconds
            // Calls render as often as possible
            static void main_loop() {
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
            static void update_game_state(){
                // Update particles and joints
                // Calculates physics only when the simulation is running
                if (simulation_running) {
                    point_manager.update();
                }
            }

            
    };

    insertion_mode_t GameStateController::insertion_mode = insertion_mode_t::PARTICLE;
    physics::Particle<float>* GameStateController::prev_joint_particle = nullptr;
    bool GameStateController::simulation_running = false;
    UIController GameStateController::ui_controller = UIController();
    ParticleJointManager<float> GameStateController::point_manager = ParticleJointManager<float>();
}