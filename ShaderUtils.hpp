#ifndef SHADERUTILS_HPP
#define SHADERUTILS_HPP

#include <vector>
#include <stdexcept>
#include "config.hpp"
#include "FileUtils.hpp"
#include "StringUtils.hpp"

namespace gl_utils {

    inline void attachShaders(GLuint) { }

    template<typename... Tp>
    inline void attachShaders(GLuint program_id, GLuint shader_id, Tp... ids) {
        glAttachShader(program_id, shader_id);
        attachShaders(program_id, ids...);
    }

    template<typename... Tp>
    inline GLuint createProgram(Tp... ids) {

        GLuint programID = glCreateProgram();
        if (programID == 0) {
            throw std::runtime_error("Unable to create program");
        }

        attachShaders(programID, ids...);
        glLinkProgram(programID);

        GLint success;
        glGetProgramiv(programID, GL_LINK_STATUS, &success);

        if (!success) {
            GLint maxLength = 0;
            glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(programID, maxLength, &maxLength, &infoLog[0]);

            glDeleteProgram(programID);
            throw std::runtime_error(toString("Unable to link shader program:\n", &infoLog[0]));
        }

        return programID;
    }

    inline GLuint createTextShader(GLenum type, std::string shaderText) {

        GLuint shaderID = glCreateShader(type);
        if (shaderID == 0) {
            throw std::runtime_error("Unable to create shader");
        }

        const char *str = shaderText.c_str();
        glShaderSource(shaderID, 1, &str, nullptr);

        glCompileShader(shaderID);
        GLint success;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

        if (!success) {
            GLint maxLength = 0;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> errorLog(maxLength);
            glGetShaderInfoLog(shaderID, maxLength, &maxLength, &errorLog[0]);

            glDeleteShader(shaderID);
            throw std::runtime_error(toString("Unable to compile shader:\n", &errorLog[0]));
        }

        return shaderID;
    }

    inline GLuint createFileShader(GLenum type, const char* shaderFile) {
        std::string shaderText = readFile(shaderFile);
        return createTextShader(type, shaderText);
    }
}

#endif /* SHADERUTILS_HPP */

