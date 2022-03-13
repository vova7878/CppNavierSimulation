#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include <stdexcept>
#include "TextureUtils.hpp"

GLuint genTexture(GLsizei width, GLsizei height, GLint internalformat, GLenum format, GLenum type, const void* data) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    if (textureID == 0) {
        throw std::runtime_error("Unable to create texture");
    }

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}