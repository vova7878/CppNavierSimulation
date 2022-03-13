#ifndef SHADERUTILS_HPP
#define SHADERUTILS_HPP

GLuint createProgram(GLuint vertexShaderId, GLuint fragmentShaderId);
GLuint createTextShader(GLenum type, const char* shaderText);
GLuint createFileShader(GLenum type, const char* shaderFile);

#endif /* SHADERUTILS_HPP */
