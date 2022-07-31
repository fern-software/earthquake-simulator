#pragma once
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <iostream>
#include <string>
#include <exception>
#include "point_manager.hpp"

#define WIDTH 640
#define HEIGHT 480
#define FPS 60

namespace game {
    class GameStateController {
        public:
            // Create empty point manager and initialize openGL window
            GameStateController(int argc, char* argv[]): point_manager(PointManager()) {
                try {
                    initGL(argc, argv);
                } catch (std::exception& e) {
                    std::cout << "Failed to initialize OpenGL" << std::endl;
                    exit(1);
                }

            }
            ~GameStateController() = default;
            
        private:
            void initGL(int argc, char* argv[]) {
                glutInit(&argc, argv);
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
                if (!GLEW_VERSION_2_1) {
                    throw std::runtime_error("OpenGL 2.1 not available");
                }
                if (glGetError() != GL_NO_ERROR) {
                    std::string error_message = "OpenGL error: " + std::string((const char*)gluErrorString(glGetError()));
                    throw std::runtime_error(error_message);
                }

                // Start GLUT main loop
                glutTimerFunc( 1000 / FPS, main_loop, 0 );
                glutMainLoop();
            }

            static void main_loop(int val) {
                // Frame logic
                update_game_state();
                render();

                // Call main loop again
                glutTimerFunc( 1000 / FPS, main_loop, val );
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


            // Called every frame
            static void update_game_state(){}

            // Called when a key is pressed
            static void keyboard(unsigned char key, int x, int y){}

            PointManager point_manager;
    };
}