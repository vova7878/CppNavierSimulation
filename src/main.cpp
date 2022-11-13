#include <iostream>
#include <chrono>

#include "Window.hpp"
#include "TextureUtils.hpp"
#include "Addition.hpp"
#include "Visualization.hpp"
#include "Screen.hpp"

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

template<typename T>
struct delay_build {
    alignas(T) char data[sizeof (T)];
    T *pointer;

    template<typename... Tp>
    void init(Tp&&... params) {
        pointer = new(&data) T(std::forward(params)...);
    }

    void destroy() {
        pointer->~T();
    }

    T* operator->() const {
        return pointer;
    }
};

struct MainRenderer : public gl_utils::Renderer {
    constexpr static int data_width = 400, data_height = 400;

    gl_utils::Window* window;
    int render_width, render_height;

    GLuint screen_buffer;
    GLuint data_buffer;

    delay_build<Visualization> vis;
    delay_build<Addition> add;
    delay_build<Screen> scr;

    virtual void onCreate(gl_utils::Window* w) override {
        window = w;

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(debug_callback, 0);

        vis.init();
        add.init();
        scr.init();

        float tmp[data_width * data_height * 2];
        for (int i = 0; i < data_width * data_height; i++) {
            tmp[i * 2 + 0] = float(data_width - i % data_width) * float(data_height - i / data_width) / data_width / data_height;
            tmp[i * 2 + 1] = float(i % data_width) * float(i / data_width) / data_width / data_height;
        }

        data_buffer = gl_utils::genBuffer(GL_SHADER_STORAGE_BUFFER, sizeof (tmp), tmp, GL_STATIC_DRAW);
        screen_buffer = gl_utils::genBuffer(GL_SHADER_STORAGE_BUFFER,
                sizeof (float) * data_width * data_height * 4, nullptr, GL_STATIC_DRAW);

    }

    virtual void onDraw() override {
        add->apply(data_buffer, data_buffer, 0, 0, 0, data_width, data_height, -0.001);
        vis->apply(data_buffer, screen_buffer, 0, data_width, data_height);
        scr->apply(screen_buffer, data_width, data_height, render_width, render_height);

        showFPS();
    }

    virtual void onChangeSize(int width, int height) override {
        render_width = width;
        render_height = height;
        glViewport(0, 0, render_width, render_height);
    }

    virtual void onDispose() override {
        vis.destroy();
        add.destroy();
        scr.destroy();
    }

    using clock_type = std::chrono::high_resolution_clock;
    using time_type = std::chrono::time_point<clock_type>;

    time_type time = clock_type::now();
    int count = 0;

    void showFPS() {
        auto tmp_time = clock_type::now();
        double value = std::chrono::duration_cast<std::chrono::duration<double>>(tmp_time - time).count();
        count++;
        if ((value > 0.1) && (count > 5)) {
            double fps = count / value;
            window->setTitle(toString("Simulation ", fps));
            time = tmp_time;
            count = 0;
        }
    }
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
