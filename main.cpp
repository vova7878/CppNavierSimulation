#include <iostream>
#include <stdexcept>
#include <chrono>

#include "config.hpp"
#include "Window.hpp"
#include "ShaderUtils.hpp"
#include "TextureUtils.hpp"

void GLAPIENTRY
debug_callback(GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei /*length*/,
        const GLchar* message,
        const void* /*userParam*/) {
    if (type == GL_DEBUG_TYPE_ERROR) {
        fprintf(stdout, "debug message:%s 0x%x, 0x%x, %u, 0x%x, \"%s\"\n",
                type == GL_DEBUG_TYPE_ERROR ? " ** GL ERROR **" : "",
                source, type, id, severity, message);
    }
}

struct MainRenderer : public gl_utils::Renderer {
    gl_utils::Window* window;
    int w_width, w_height;
    GLuint vaoID;
    GLuint vboID;
    GLint positionLocation;
    GLint renderSizeLocation;

    virtual void onCreate(gl_utils::Window* w) override {
        window = w;

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debug_callback, 0);

        int vert = gl_utils::createFileShader(GL_VERTEX_SHADER, "shaders/simple.vert");
        int frag = gl_utils::createFileShader(GL_FRAGMENT_SHADER, "shaders/screen.frag");
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

    int i;
    decltype(std::chrono::high_resolution_clock::now())
    time = std::chrono::high_resolution_clock::now();

    virtual void onDraw() override {
        glViewport(0, 0, w_width, w_height);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vaoID);
        glEnableVertexAttribArray(positionLocation);

        glUniform2i(renderSizeLocation, w_width, w_height);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableVertexAttribArray(positionLocation);
        glBindVertexArray(0);

        //TODO
        i++;
        auto tmp_time = std::chrono::high_resolution_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(tmp_time - time).count();
        if (delta > 100000000) {
            time = tmp_time;
            double fps = i * 1000000000.0 / delta;
            window->setTitle(toString("Simulation ", fps));
            i = 0;
        }
    }

    virtual void onChangeSize(int width, int height) override {
        w_width = width;
        w_height = height;
    }

    virtual void onDispose() override { }
};

struct EmptyRenderer : public gl_utils::Renderer {

    virtual void onDraw() override {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
};

int main(int, char**) {

    gl_utils::init();

    MainRenderer r1;
    gl_utils::Window w1(&r1);
    w1.setSize(500, 500);
    //w1.setDefaultLocation();
    w1.setVisible(true);

    MainRenderer r2;
    gl_utils::Window w2(&r2);
    w2.setSize(500, 500);
    //w2.setDefaultLocation();
    w2.setVisible(true);

    MainRenderer r3;
    gl_utils::Window w3(&r3);
    w3.setSize(500, 500);
    //w3.setDefaultLocation();
    w3.setVisible(true);

    w1.waitClose();
    w2.waitClose();
    w3.waitClose();

    gl_utils::terminate();
}
