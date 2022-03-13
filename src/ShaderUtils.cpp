#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include <cstring>
#include <stdexcept>
#include "FileUtils.hpp"
#include "ShaderUtils.hpp"

GLuint createProgram(GLuint vertexShaderId, GLuint fragmentShaderId) {
    GLuint programID = glCreateProgram();
    if (vertexShaderId) {
        glAttachShader(programID, vertexShaderId);
    }
    if (fragmentShaderId) {
        glAttachShader(programID, fragmentShaderId);
    }
    glLinkProgram(programID);
    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        glDeleteProgram(programID);
        throw std::runtime_error("Unable to link shader program");
    }
    return programID;
}

GLuint createTextShader(GLenum type, const char* shaderText) {
    GLuint shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &shaderText, nullptr);
    glCompileShader(shaderID);
    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512] = "Error creating shader:\n";
        int length = std::strlen(infoLog);
        glGetShaderInfoLog(shaderID, 512 - length, nullptr, infoLog + length);
        throw std::runtime_error(infoLog);
    }
    return shaderID;
}

GLuint createFileShader(GLenum type, const char* shaderFile) {
    char* shaderText = readFile(shaderFile);
    GLuint out = createTextShader(type, shaderText);
    delete[] shaderText;
    return out;
}