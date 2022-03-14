#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "glad/glad.h"
#include "ShaderUtils.hpp"
#include "TextureUtils.hpp"
#include "Condition.hpp"
#include "Renderer.hpp"

void Renderer::onCreate() {
    widthR = WIDTH;
    heightR = HEIGHT;
    dt = DT;
    diffusion = DIFFUSION;
    viscosity = VISCOSITY;

    int vertexShaderId = createFileShader(GL_VERTEX_SHADER, "shaders/rectangle.vert");
    // общий вершинный шейдер есть, а общего фрагментного нет
    int programID = createProgram(vertexShaderId, 0);

    vis = new Visualization(vertexShaderId);
    add = new Addition(vertexShaderId);
    adv = new Advection(vertexShaderId);
    div = new Divergence(vertexShaderId);
    press = new Pressure(vertexShaderId);
    project = new Projection(vertexShaderId);
    diff = new Diffusion(vertexShaderId);

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    float vertices[] = {
        -1, -1,
        1, -1,
        1, 1,
        -1, 1
    };

    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);

    positionLocation = glGetAttribLocation(programID, "position");
    glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, 0, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);

    GLint internalFormat = GL_RG32F;
    GLenum format = GL_RG;

    for (int i = 0; i < sizeof (tmpTextureIDs) / sizeof (tmpTextureIDs[0]); i++) {
        tmpTextureIDs[i] = genTexture(widthR, heightR, internalFormat, format, GL_FLOAT, nullptr);
    }

    pTextureID = genTexture(widthR, heightR, internalFormat, format, GL_FLOAT, nullptr);

    vTextureID = genTexture(widthR, heightR, internalFormat, format, GL_FLOAT, nullptr);

    float tmp[WIDTH * HEIGHT * 2];

    for (int i = 0; i < WIDTH * HEIGHT * 2; i++) {
        tmp[i] = 1.0f;
    }

    for (int i = 0; i < sizeof (cTextureIDs) / sizeof (cTextureIDs[0]); i++) {
        cTextureIDs[i] = genTexture(widthR, heightR, internalFormat, format, GL_FLOAT, tmp);
    }

    for (int i = 0; i < sizeof (sTextureIDs) / sizeof (sTextureIDs[0]); i++) {
        sTextureIDs[i] = genTexture(widthR, heightR, internalFormat, format, GL_FLOAT, getS(tmp, i));
    }

    fTextureID = genTexture(widthR, heightR, internalFormat, format, GL_FLOAT, getF(tmp));

    displayTextureID = genTexture(widthR, heightR, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    GLuint bufs[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, bufs);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, displayTextureID, 0);

    glViewport(0, 0, widthR, heightR);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framefuffer is not complete");
    }
}

void Renderer::drawStages() {
    for (int i = 0; i < sizeof (cTextureIDs) / sizeof (cTextureIDs[0]); i++) {
        add->apply(cTextureIDs[i], sTextureIDs[i], tmpTextureIDs[0], widthR, heightR, dt,
                false, false, false, false);

        if (diffusion != 0.0f) {
            diff->apply(cTextureIDs[i], tmpTextureIDs[0], tmpTextureIDs[1],
                    widthR, heightR, diffusion * dt, false, false, false, false);
        }

        adv->apply(cTextureIDs[i], vTextureID, tmpTextureIDs[0], widthR, heightR, dt,
                false, false, false, false);
    }

    add->apply(vTextureID, fTextureID, tmpTextureIDs[0], widthR, heightR, dt,
            true, false, false, true);

    if (viscosity != 0.0f) {
        diff->apply(vTextureID, tmpTextureIDs[0], tmpTextureIDs[1],
                widthR, heightR, viscosity * dt, true, false, false, true);
    }

    adv->apply(vTextureID, vTextureID, tmpTextureIDs[0], widthR, heightR, dt,
            true, false, false, true);

    //записываем дивергенцию в красный канал tmpTextureIDs[0]
    div->apply(vTextureID, tmpTextureIDs[0], widthR, heightR);
    //записываем давление в красный канал tmpTextureIDs[2]
    press->apply(tmpTextureIDs[0], pTextureID, tmpTextureIDs[1], widthR, heightR);
    //применияем градиент давления
    project->apply(vTextureID, pTextureID, tmpTextureIDs[1],
            widthR, heightR, true, false, false, true);

    vis->apply(cTextureIDs[0], cTextureIDs[1], displayTextureID, widthR, heightR);
}

void Renderer::onDraw() {
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    glClearColor(0.75f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vaoID);
    glEnableVertexAttribArray(positionLocation);

    drawStages();

    glDisableVertexAttribArray(positionLocation);
    glBindVertexArray(0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int x = (widthS - widthR) / 2, y = (heightS - heightR) / 2;

    glBlitFramebuffer(0, 0, widthR, heightR, x, y, x + widthR, y + heightR,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Renderer::onChangeSize(GLFWwindow* window, int width, int height) {
    widthS = width;
    heightS = height;
}

void Renderer::onChangeCursorPos(GLFWwindow* window, double xpos, double ypos) {
}

void Renderer::onMouseButtonEvent(GLFWwindow* window, int button, int action, int mods) {
}

void Renderer::onScroll(GLFWwindow* window, double xoffset, double yoffset) {
}

void Renderer::onKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
}

void Renderer::onDispose() {
}
