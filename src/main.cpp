#include <iostream>
#include <stdexcept>
#include <chrono>

#include "config.hpp"
#include "Window.hpp"
#include "ShaderUtils.hpp"
#include "TextureUtils.hpp"

template<typename T>
struct Counter {
private:
    size_t elements;
    T sum;

public:

    Counter() : elements(0), sum(0) { }

    void push(T v) {
        elements++;
        sum += v;
    }

    void clear() {
        elements = 0;
        sum = 0;
    }

    T get() {
        T out = sum / elements;
        clear();
        return out;
    }
};

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
    int render_width, render_height;
    GLuint vaoID;
    GLuint vboID;
    GLint positionLocation;
    GLint dataSizeLocation;
    GLint renderSizeLocation;

    GLuint programID1;
    GLuint programID2;

    constexpr static int data_width = 400, data_height = 400;

    virtual void onCreate(gl_utils::Window* w) override {
        window = w;

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debug_callback, 0);

        int comp = gl_utils::createFileShader(GL_COMPUTE_SHADER, "shaders/compute.comp");
        programID2 = gl_utils::createProgram(comp);

        int vert = gl_utils::createFileShader(GL_VERTEX_SHADER, "shaders/simple.vert");
        int frag = gl_utils::createFileShader(GL_FRAGMENT_SHADER, "shaders/screen.frag");
        programID1 = gl_utils::createProgram(vert, frag);

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

        positionLocation = glGetAttribLocation(programID1, "position");
        glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, 0, 0);

        glBindVertexArray(0);

        float tmp[data_width * data_height];
        for (int i = 0; i < data_width * data_height; i++) {
            tmp[i] = float(data_width - i % data_width) * float(i / data_width) / data_width / data_height;
        }

        GLint ssbo = gl_utils::genBuffer(GL_SHADER_STORAGE_BUFFER, sizeof (tmp), tmp, GL_STATIC_DRAW);

        GLint block_index = glGetProgramResourceIndex(programID1, GL_SHADER_STORAGE_BLOCK, "ssbo1");
        glShaderStorageBlockBinding(programID1, block_index, 1);

        block_index = glGetProgramResourceIndex(programID2, GL_SHADER_STORAGE_BLOCK, "ssbo1");
        glShaderStorageBlockBinding(programID2, block_index, 1);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);

        dataSizeLocation = glGetUniformLocation(programID1, "dataSize");
        renderSizeLocation = glGetUniformLocation(programID1, "renderSize");
    }

    using clock_type = std::chrono::high_resolution_clock;
    using time_type = std::chrono::time_point<clock_type>;

    time_type time1 = clock_type::now();
    time_type time2 = time1;
    Counter<double> f;

    void showFPS() {
        auto tmp_time = clock_type::now();
        f.push(std::chrono::duration_cast<std::chrono::duration<double>>(tmp_time - time1).count());
        time1 = tmp_time;
        if (std::chrono::duration_cast<std::chrono::duration<double>>(tmp_time - time2).count() > 0.1) {
            double fps = 1.0 / f.get();
            window->setTitle(toString("Simulation ", fps));
            time2 = tmp_time;
        }
    }

    virtual void onDraw() override {
        glUseProgram(programID2);

        glDispatchCompute(400, 400, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glUseProgram(programID1);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vaoID);
        glEnableVertexAttribArray(positionLocation);

        glUniform2i(dataSizeLocation, data_width, data_height);
        glUniform2i(renderSizeLocation, render_width, render_height);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableVertexAttribArray(positionLocation);
        glBindVertexArray(0);

        showFPS();
    }

    virtual void onChangeSize(int width, int height) override {
        render_width = width;
        render_height = height;
        glViewport(0, 0, render_width, render_height);
    }

    virtual void onDispose() override { }
};

int main(int, char**) {

    gl_utils::init();

    auto wh = gl_utils::defaultWindowHints();

    wh.setSize(500, 500);
    wh.setSwapInterval(1);

    MainRenderer r1;
    gl_utils::Window w1(&r1, wh);

    w1.setVisible(true);

    w1.waitClose();

    gl_utils::terminate();
}
