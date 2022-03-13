#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include "glad/glad.h"
#include "ShaderUtils.hpp"
#include "Divergence.hpp"

Divergence::Divergence(int vertex_shader) {
    const char* fragmentFile = "shaders/divergence.frag";
    int fragment_shader = createFileShader(GL_FRAGMENT_SHADER, fragmentFile);

    programID = createProgram(vertex_shader, fragment_shader);

    renderSizeLocation = glGetUniformLocation(programID, "renderSize");

    inDataLocation = glGetUniformLocation(programID, "inData");
}

void Divergence::setInput(GLuint texID) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texID);

    glActiveTexture(GL_TEXTURE0);
}

void Divergence::setOutput(GLuint texID) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
}

void Divergence::apply(GLuint vTextureID, GLuint outTextureID,
        int width, int height) {
    glUseProgram(programID);
    glUniform2i(renderSizeLocation, width, height);
    glUniform1i(inDataLocation, 1);
    GLuint bufs[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, bufs);
    setInput(vTextureID);
    setOutput(outTextureID);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
