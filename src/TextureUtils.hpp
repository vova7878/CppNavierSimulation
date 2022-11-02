#ifndef TEXTUREUTILS_HPP
#define TEXTUREUTILS_HPP

#include <stdexcept>
#include "config.hpp"

namespace gl_utils {

    inline GLuint currentTexture2D() {
        GLint texID;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &texID);
        return texID;
    }

    inline GLuint currentTextureUnit() {
        GLint unitID;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &unitID);
        return unitID - GL_TEXTURE0;
    }

    inline void activeTextureUnit(GLuint unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
    }

    inline void bindTexture(GLuint unit, GLenum target, GLuint texture) {
        GLuint tmp_unit = currentTextureUnit();
        activeTextureUnit(unit);

        glBindTexture(target, texture);

        activeTextureUnit(tmp_unit);
    }

    inline GLuint genTexture2D(GLsizei width, GLsizei height, GLint internalformat,
            GLenum format, GLenum type, const void* data,
            GLint wrap_s = GL_CLAMP_TO_EDGE, GLint wrap_t = GL_CLAMP_TO_EDGE,
            GLint min_filter = GL_LINEAR, GLint mag_filter = GL_LINEAR) {

        GLuint textureID;
        glGenTextures(1, &textureID);

        if (textureID == 0) {
            throw std::runtime_error("Unable to create texture");
        }

        GLuint tmp_texture = currentTexture2D();
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);

        glBindTexture(GL_TEXTURE_2D, tmp_texture);

        return textureID;
    }

    inline GLuint genBuffer(GLenum target, GLsizeiptr size, const void *data, GLenum usage) {

        GLuint bufferID;
        glGenBuffers(1, &bufferID);

        if (bufferID == 0) {
            throw std::runtime_error("Unable to create buffer");
        }

        glBindBuffer(target, bufferID);
        glBufferData(target, size, data, usage);

        glBindBuffer(target, 0);

        return bufferID;
    }
}

#endif /* TEXTUREUTILS_HPP */

