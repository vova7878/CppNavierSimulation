#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "ShaderUtils.hpp"
#include "Visualization.hpp"

Visualization::Visualization(int vertex_shader) {
    const char* fragmentFile = "shaders/visualization.frag";
    int fragment_shader = createFileShader(GL_FRAGMENT_SHADER, fragmentFile);

    programID = createProgram(vertex_shader, fragment_shader);

    renderSizeLocation = glGetUniformLocation(programID, "renderSize");

    inData1Location = glGetUniformLocation(programID, "inData1");
    inData2Location = glGetUniformLocation(programID, "inData2");
}

void Visualization::setInputs(GLuint tex1ID, GLuint tex2ID) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1ID);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex2ID);

    glActiveTexture(GL_TEXTURE0);
}

void Visualization::setOutput(GLuint texID) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
}

void Visualization::apply(GLuint cTextureID, GLuint vTextureID, GLuint displayTextureID,
        int width, int height) {
    glUseProgram(programID);
    glUniform2i(renderSizeLocation, width, height);
    glUniform1i(inData1Location, 1);
    glUniform1i(inData2Location, 2);
    GLuint bufs[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, bufs);
    setInputs(cTextureID, vTextureID);
    setOutput(displayTextureID);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

