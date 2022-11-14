#ifndef ADVECTION_HPP
#define ADVECTION_HPP

#include "config.hpp"
#include "ShaderUtils.hpp"

class Advection {
private:
    GLuint programID;

    GLint offsetLocation;
    GLint sizeLocation;
    GLint typeLocation;
    GLint dtLocation;
public:

    Advection() {
        GLuint comp_shader = gl_utils::createFileShader(GL_COMPUTE_SHADER, "shaders/advection.comp");
        programID = gl_utils::createProgram(comp_shader);
        glDeleteShader(comp_shader);

        offsetLocation = glGetUniformLocation(programID, "offset");
        sizeLocation = glGetUniformLocation(programID, "size");
        typeLocation = glGetUniformLocation(programID, "type");
        dtLocation = glGetUniformLocation(programID, "dt");

        GLint buf0_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "buf0");
        GLint buf1_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "buf1");
        GLint buf2_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "buf2");
        GLint buf3_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "buf3");

        glShaderStorageBlockBinding(programID, buf0_index, 1);
        glShaderStorageBlockBinding(programID, buf1_index, 2);
        glShaderStorageBlockBinding(programID, buf2_index, 3);
        glShaderStorageBlockBinding(programID, buf3_index, 4);
    }

    ~Advection() {
        glDeleteProgram(programID);
    }

    void apply(GLuint velocity_buffer, GLuint &data_buffer, GLuint &tmp_buffer,
    GLuint mask_buffer, int x, int y, int width, int height, int type, float dt) {

        glUseProgram(programID);

        glUniform2i(offsetLocation, x, y);
        glUniform2i(sizeLocation, width, height);
        glUniform1i(typeLocation, type);
        glUniform1f(dtLocation, dt);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocity_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, data_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, tmp_buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, mask_buffer);

        glDispatchCompute(width - x, height - x, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        std::swap(data_buffer, tmp_buffer);
    }
};

#endif /* ADVECTION_HPP */
