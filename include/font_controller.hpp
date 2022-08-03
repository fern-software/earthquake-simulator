#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>
#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>
#include <pango/pangocairo.h>


namespace game {
    #define FONT "Sans Bold 18"

    // Adapted for C++ from https://dthompson.us/font-rendering-in-opengl-with-pango-and-cairo.html
    // This class is used to render text to the screen.
    class FontController {
        private:
            struct texture_info_t {
                int id;
                int width;
                int height;
            };

            std::map<std::string, texture_info_t> textures;

            // Creates a texture and returns its OpenGL ID
            unsigned int create_texture (unsigned int width, unsigned int height, unsigned char *pixels) {
                unsigned int texture_id;

                glGenTextures(1, &texture_id);
                glBindTexture(GL_TEXTURE_2D, texture_id);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexImage2D(GL_TEXTURE_2D,
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

            void draw_texture(int x, int y, int width, int height, unsigned int texture_id) {
                /* Render a texture in immediate mode. */
                int x_top = x + width;
                int y_top = y + height;
                glEnable (GL_BLEND);
                glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glEnable (GL_TEXTURE_2D);
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

            inline void get_text_size(PangoLayout *layout, int *width, int *height) {
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

        public:
            // Renders the given text to the screen at (x,y). Text is cached for performance but new strings can be slow on the first sighting
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
            }
    };
}