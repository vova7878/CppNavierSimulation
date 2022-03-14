#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include "glad/glad.h"
#include "ShaderUtils.hpp"
#include "Diffusion.hpp"

#define ITERATIONS_COUNT 20

Diffusion::Diffusion(int vertex_shader) {
    const char* fragmentFile = "shaders/diffusion.frag";
    int fragment_shader = createFileShader(GL_FRAGMENT_SHADER, fragmentFile);

    programID = createProgram(vertex_shader, fragment_shader);

    diffValueLocation = glGetUniformLocation(programID, "diffValue");

    invertEdgesLocation = glGetUniformLocation(programID, "invertEdges");

    renderSizeLocation = glGetUniformLocation(programID, "renderSize");

    inData1Location = glGetUniformLocation(programID, "inData1");
    inData2Location = glGetUniformLocation(programID, "inData2");
}

void Diffusion::setEdgeInversion(bool invert1X, bool invert1Y, bool invert2X, bool invert2Y) {
    glUniform4i(invertEdgesLocation, invert1X, invert1Y, invert2X, invert2Y);
}

void Diffusion::setInputs(GLuint tex1ID, GLuint tex2ID) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1ID);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex2ID);

    glActiveTexture(GL_TEXTURE0);
}

void Diffusion::setOutput(GLuint texID) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
}

void Diffusion::apply(GLuint& dataTextureID, GLuint& tmp1TextureID, GLuint& tmp2TextureID,
        int width, int height, float diffValue,
        bool invert1X, bool invert1Y, bool invert2X, bool invert2Y) {

    glUseProgram(programID);
    glUniform2i(renderSizeLocation, width, height);
    glUniform1i(inData1Location, 1);
    glUniform1i(inData2Location, 2);
    glUniform1f(diffValueLocation, diffValue);
    setEdgeInversion(invert1X, invert1Y, invert2X, invert2Y);
    GLuint bufs[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, bufs);

    for (int i = 0; i < ITERATIONS_COUNT; i++) {
        setInputs(dataTextureID, tmp1TextureID);
        setOutput(tmp2TextureID);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        std::swap(tmp1TextureID, tmp2TextureID);
    }

    std::swap(dataTextureID, tmp1TextureID);
}
