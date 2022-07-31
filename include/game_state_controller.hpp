#pragma once
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <string>
#include "point_manager.hpp"

#define WIDTH 640
#define HEIGHT 480
#define FPS 60

namespace game {
    class GameStateController {
        public:
            // Create empty point manager and initialize openGL window
            GameStateController(): point_manager(PointManager()) {
                try {
                    initGL();
                } catch (std::exception& e) {
                    std::cout << "Failed to initialize OpenGL" << std::endl;
                    exit(1);
                }

            }
            ~GameStateController();
            
        private:
            void initGL() {
                glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
                glutInitWindowSize(WIDTH, HEIGHT);
                glutInitWindowPosition(100, 100);
                glutCreateWindow("Earthquake Simulator");
                glutDisplayFunc(render);
                glutIdleFunc(update_game_state);
                glutKeyboardFunc(keyboard);
                glewInit();
                
                //Initialize Projection Matrix
                glMatrixMode( GL_PROJECTION );
                glLoadIdentity();
                //Initialize Modelview Matrix
                glMatrixMode( GL_MODELVIEW );
                glLoadIdentity();
                //Initialize clear color to black
                glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );

                //Check for errors
                if (!GLEW_VERSION_2_1)              throw std::runtime_error("OpenGL 2.1 not available");
                if (glGetError() != GL_NO_ERROR) {
                    std::string error_message = "OpenGL error: " + std::string((const char*)gluErrorString(glGetError()));
                    throw std::runtime_error(error_message);
                }
            }

            // Render the current state of the simulation
            // PRECONDITION:
            //     - OpenGL context must be initialized
            //     - Active modelview matrix
            static void render(){
                //Clear color buffer
                glClear( GL_COLOR_BUFFER_BIT );

                //Render quad
                glBegin( GL_QUADS );
                    glVertex2f( -0.5f, -0.5f );
                    glVertex2f(  0.5f, -0.5f );
                    glVertex2f(  0.5f,  0.5f );
                    glVertex2f( -0.5f,  0.5f );
                glEnd();

                //Update screen
                glutSwapBuffers();
            }


            // Updates the display with the current state of the simulation
            // Called every frame
            static void update_game_state(){}

            // Called when a key is pressed
            static void keyboard(unsigned char key, int x, int y){}

            PointManager point_manager;
    };
}