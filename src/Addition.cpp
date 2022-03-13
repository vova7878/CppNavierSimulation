#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <algorithm>
#include "glad/glad.h"
#include "ShaderUtils.hpp"
#include "Addition.hpp"

Addition::Addition(int vertex_shader) {
    const char* fragmentFile = "shaders/addition.frag";
    int fragment_shader = createFileShader(GL_FRAGMENT_SHADER, fragmentFile);

    programID = createProgram(vertex_shader, fragment_shader);

    dtLocation = glGetUniformLocation(programID, "dt");

    invertEdgesLocation = glGetUniformLocation(programID, "invertEdges");

    renderSizeLocation = glGetUniformLocation(programID, "renderSize");

    inData1Location = glGetUniformLocation(programID, "inData1");
    inData2Location = glGetUniformLocation(programID, "inData2");
}

void Addition::setEdgeInversion(bool invert1X, bool invert1Y, bool invert2X, bool invert2Y) {
    glUniform4i(invertEdgesLocation, invert1X, invert1Y, invert2X, invert2Y);
}

void Addition::setInputs(GLuint tex1ID, GLuint tex2ID) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1ID);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex2ID);

    glActiveTexture(GL_TEXTURE0);
}

void Addition::setOutput(GLuint texID) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
}

void Addition::apply(GLuint& dataTextureID, GLuint deltaTextureID, GLuint& tmpTextureID,
        int width, int height, float dt,
        bool invert1X, bool invert1Y, bool invert2X, bool invert2Y) {
    glUseProgram(programID);
    glUniform2i(renderSizeLocation, width, height);
    glUniform1i(inData1Location, 1);
    glUniform1i(inData2Location, 2);
    glUniform1f(dtLocation, dt);
    setEdgeInversion(invert1X, invert1Y, invert2X, invert2Y);
    GLuint bufs[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, bufs);
    setInputs(dataTextureID, deltaTextureID);
    setOutput(tmpTextureID);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    std::swap(dataTextureID, tmpTextureID);
}
