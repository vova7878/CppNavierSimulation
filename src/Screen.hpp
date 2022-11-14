#ifndef SCREEN_HPP
#define SCREEN_HPP

#include "config.hpp"
#include "ShaderUtils.hpp"

class Screen {
private:
    GLuint programID;

    GLuint vaoID;
    GLuint vboID;
    GLint positionLocation;
    GLint dataSizeLocation;
    GLint renderSizeLocation;
public:

    Screen() {
        int vert = gl_utils::createFileShader(GL_VERTEX_SHADER, "shaders/simple.vert");
        int frag = gl_utils::createFileShader(GL_FRAGMENT_SHADER, "shaders/screen.frag");
        programID = gl_utils::createProgram(vert, frag);
        glDeleteShader(vert);
        glDeleteShader(frag);

        positionLocation = glGetAttribLocation(programID, "position");
        dataSizeLocation = glGetUniformLocation(programID, "dataSize");
        renderSizeLocation = glGetUniformLocation(programID, "renderSize");

        glGenVertexArrays(1, &vaoID);
        glBindVertexArray(vaoID);

        float vertices[] = {
            -1, -1,
            1, -1,
            1, 1,
            -1, 1
        };

        vboID = gl_utils::genBuffer(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, 0, 0);

        glBindVertexArray(0);

        GLint block_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "ssbo1");
        glShaderStorageBlockBinding(programID, block_index, 1);
    }

    ~Screen() {
        glDeleteBuffers(1, &vboID);
        glDeleteVertexArrays(1, &vaoID);
        glDeleteProgram(programID);
    }

    void apply(GLuint screen_buffer, int data_width, int data_height,
    int render_width, int render_height) {

        glUseProgram(programID);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, screen_buffer);

        glUniform2i(dataSizeLocation, data_width, data_height);
        glUniform2i(renderSizeLocation, render_width, render_height);

        glBindVertexArray(vaoID);
        glEnableVertexAttribArray(positionLocation);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableVertexAttribArray(positionLocation);
        glBindVertexArray(0);
    }
};

#endif /* SCREEN_HPP */
