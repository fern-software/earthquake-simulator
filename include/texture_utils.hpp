#include <GL/glew.h>
#include <GL/glu.h>
#include <GLFW/glfw3.h>

namespace texture_utils {
    struct texture_info_t {
        unsigned int id;
        unsigned int width;
        unsigned int height;
    };

    // Creates a texture and returns its OpenGL ID
    unsigned int create_texture(unsigned int width, unsigned int height, unsigned char *pixels, int channels) {
        unsigned int texture_id;

        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     channels,
                     width,
                     height,
                     0,
                     channels,
                     GL_UNSIGNED_BYTE,
                     pixels);

        // glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
        return texture_id;
    }

    // Render a textured quad
    // Width and height override the texture's width and height and scale it appropriately
    void draw_texture(int x, int y, texture_info_t texture, int width = 0, int height = 0) {
        int x_top = x + (width == 0 ? texture.width : width);
        int y_top = y + (height == 0 ? texture.height : height);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, texture.id);
        glEnable(GL_TEXTURE_2D);
        glPushMatrix();
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

    // Load a texture from a file and return a texture_info_t
    texture_info_t load_texture(const char *filename, int width = 0, int height = 0) {
        unsigned int texture_id;
        int channels;

        // Read texture from disk
        FILE *fp = fopen(filename, "r");
        if (fp == NULL) {
            throw std::runtime_error("Could not open texture file");
        }
        fseek(fp , 0 , SEEK_END);
        int size = ftell(fp);
        unsigned char *pixels = (unsigned char*)malloc(sizeof(unsigned char) * size);
        rewind(fp);
        fread(pixels, sizeof(unsigned char), size, fp);
        fclose(fp);

        if (!pixels) {
            throw std::runtime_error("Failed to load texture");
        }

        texture_id = create_texture(width, height, pixels, GL_RGB);
        free(pixels);
        return {texture_id, width, height};
    }
}