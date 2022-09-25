#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "config.hpp"
#include "ShaderUtils.hpp"
#include "TextureUtils.hpp"

struct Renderer {
    int w_width, w_height;
    GLuint vaoID;
    GLuint vboID;
    GLint positionLocation;
    GLint renderSizeLocation;

    void onCreate() {

        int vert = gl_utils::createFileShader(GL_VERTEX_SHADER, "shaders/vertex.glsl");
        int frag = gl_utils::createFileShader(GL_FRAGMENT_SHADER, "shaders/screen.glsl");
        int programID = gl_utils::createProgram(vert, frag);
        glUseProgram(programID);

        glGenVertexArrays(1, &vaoID);
        glBindVertexArray(vaoID);

        float vertices[] = {
            -1, -1,
            1, -1,
            1, 1,
            -1, 1
        };

        vboID = gl_utils::genBuffer(GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);

        positionLocation = glGetAttribLocation(programID, "position");
        glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, 0, 0);

        glBindVertexArray(0);

        constexpr int width = 400, height = 400;

        float tmp[width * height];
        for (int i = 0; i < width * height; i++) {
            tmp[i] = float(i) / (width * height);
            //tmp[i] = 1;
        }

        GLint dataSizeLocation = glGetUniformLocation(programID, "dataSize");
        glUniform2i(dataSizeLocation, width, height);
        renderSizeLocation = glGetUniformLocation(programID, "renderSize");

        GLint ssbo = gl_utils::genBuffer(GL_SHADER_STORAGE_BUFFER, sizeof (tmp), tmp, GL_STATIC_DRAW);

        GLint block_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "ssbo1");
        glShaderStorageBlockBinding(programID, block_index, 1);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
    }

    void onDraw() {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vaoID);
        glEnableVertexAttribArray(positionLocation);

        glUniform2i(renderSizeLocation, w_width, w_height);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableVertexAttribArray(positionLocation);
        glBindVertexArray(0);
    }

    void onChangeSize(GLFWwindow* /*window*/, int width, int height) {
        w_width = width;
        w_height = height;
        glViewport(0, 0, width, height);
    }

    void onChangeCursorPos(GLFWwindow* /*window*/, double /*xpos*/, double /*ypos*/) { }

    void onMouseButtonEvent(GLFWwindow* /*window*/, int /*button*/, int /*action*/, int /*mods*/) { }

    void onScroll(GLFWwindow* /*window*/, double /*xoffset*/, double /*yoffset*/) { }

    void onKeyEvent(GLFWwindow* /*window*/, int /*key*/, int /*scancode*/, int /*action*/, int /*mods*/) { }

    void onDispose() { }
};

#endif /* RENDERER_HPP */

