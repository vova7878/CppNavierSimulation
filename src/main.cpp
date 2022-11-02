#include <iostream>
#include <stdexcept>
#include <chrono>

#include "config.hpp"
#include "Window.hpp"
#include "ShaderUtils.hpp"
#include "TextureUtils.hpp"

template<typename T, size_t size>
struct FastLinearFilter {
private:
    size_t index;
    size_t elements;
    T sum;
    T buff[size];

public:

    FastLinearFilter() : index(0), elements(0), sum(0) {
        for (size_t i = 0; i < size; i++) {
            buff[i] = 0;
        }
    }

    void push(double v) {
        if (elements < size) {
            elements++;
        }
        sum += v - buff[index];
        buff[index] = v;
        index = (index + 1) % size;
    }

    void resum() {
        sum = 0;
        for (size_t i = 0; i < size; i++) {
            sum += buff[i];
        }
    }

    T get() {
        return sum / elements;
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
            tmp[i] = float(width - i % width) * float(i / width) / width / height;
        }

        GLint dataSizeLocation = glGetUniformLocation(programID, "dataSize");
        glUniform2i(dataSizeLocation, width, height);
        renderSizeLocation = glGetUniformLocation(programID, "renderSize");

        GLint ssbo = gl_utils::genBuffer(GL_SHADER_STORAGE_BUFFER, sizeof (tmp), tmp, GL_STATIC_DRAW);

        GLint block_index = glGetProgramResourceIndex(programID, GL_SHADER_STORAGE_BLOCK, "ssbo1");
        glShaderStorageBlockBinding(programID, block_index, 1);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo);
    }

    decltype(std::chrono::high_resolution_clock::now())
    time1 = std::chrono::high_resolution_clock::now();
    decltype(std::chrono::high_resolution_clock::now())
    time2 = std::chrono::high_resolution_clock::now();
    FastLinearFilter<double, 64> f;

    void showFPS() {
        auto tmp_time = std::chrono::high_resolution_clock::now();
        f.push(std::chrono::duration_cast<std::chrono::duration<double>>(tmp_time - time1).count());
        time1 = tmp_time;
        if (std::chrono::duration_cast<std::chrono::duration<double>>(tmp_time - time2).count() > 0.1) {
            double fps = 1.0 / f.get();
            window->setTitle(toString("Simulation ", fps));
            time2 = tmp_time;
        }
    }

    virtual void onDraw() override {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vaoID);
        glEnableVertexAttribArray(positionLocation);

        glUniform2i(renderSizeLocation, w_width, w_height);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableVertexAttribArray(positionLocation);
        glBindVertexArray(0);

        showFPS();
    }

    virtual void onChangeSize(int width, int height) override {
        w_width = width;
        w_height = height;
        glViewport(0, 0, w_width, w_height);
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
