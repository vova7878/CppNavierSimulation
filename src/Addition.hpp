#ifndef ADDITION_HPP
#define ADDITION_HPP

#include "config.hpp"
#include "ShaderUtils.hpp"

class Addition {
private:
    GLuint programID;

    GLint offsetLocation;
    GLint sizeLocation;
    GLint kLocation;
public:

    Addition() {
        programID = gl_utils::createProgram(
        gl_utils::createFileShader(GL_COMPUTE_SHADER, "shaders/addition.comp"));

        offsetLocation = glGetUniformLocation(programID, "offset");
        sizeLocation = glGetUniformLocation(programID, "size");
        kLocation = glGetUniformLocation(programID, "k");

        GLint buf0_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "buf0");
        GLint buf1_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "buf1");
        GLint buf2_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "buf2");

        glShaderStorageBlockBinding(programID, buf0_index, 1);
        glShaderStorageBlockBinding(programID, buf1_index, 2);
        glShaderStorageBlockBinding(programID, buf2_index, 3);
    }

    void apply(GLuint data_buffer, GLuint delta_buffer, GLuint mask_buffer,
    int x, int y, int width, int height, float k) {

        glUseProgram(programID);

        glUniform2i(offsetLocation, x, y);
        glUniform2i(sizeLocation, width, height);
        glUniform1f(kLocation, k);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, data_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, delta_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mask_buffer);

        glDispatchCompute(width - x, height - x, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
};

#endif /* ADDITION_HPP */
