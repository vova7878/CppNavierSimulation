#ifndef VISUALIZATION_HPP
#define VISUALIZATION_HPP

#include "config.hpp"
#include "ShaderUtils.hpp"

class Visualization {
private:
    GLuint programID;
public:

    Visualization() {
        GLuint comp_shader = gl_utils::createFileShader(GL_COMPUTE_SHADER, "shaders/visualization.comp");
        programID = gl_utils::createProgram(comp_shader);
        glDeleteShader(comp_shader);

        GLint buf0_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "buf0");
        GLint buf1_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "buf1");
        GLint buf2_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "buf2");

        glShaderStorageBlockBinding(programID, buf0_index, 1);
        glShaderStorageBlockBinding(programID, buf1_index, 2);
        glShaderStorageBlockBinding(programID, buf2_index, 3);
    }

    ~Visualization() {
        glDeleteProgram(programID);
    }

    void apply(GLuint data_buffer, GLuint output_buffer, GLuint mask_buffer,
    int width, int height) {

        glUseProgram(programID);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, data_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, output_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mask_buffer);

        glDispatchCompute(width, height, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
};

#endif /* VISUALIZATION_HPP */
