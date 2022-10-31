#include "Window.hpp"
#include <thread>
#include <future>
#include <atomic>
#include <functional>
#include "config.hpp"
#include "ThreadPool.hpp"

namespace gl_utils {

    //TODO
    //glfwSetKeyCallback(window, key_callback);
    //glfwSetCursorPosCallback(window, cursor_pos_callback);
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glfwSetScrollCallback(window, scroll_callback);

    alignas(pool_type) char pool_buf[sizeof (pool_type)];
    pool_type* gui_pool;

    void recursive_poll() {
        glfwWaitEventsTimeout(0.01);
        if (gui_pool->isOK()) {
            sendToUIThread(recursive_poll);
        }
    }

    void error_callback(int error, const char* description) {
        fprintf(stderr, "Error code: %i, text: %s\n", error, description);
    }

    void init() {
        gui_pool = new(&pool_buf) pool_type(16, []() {
            glfwSetErrorCallback(error_callback);

            if (!glfwInit()) {
                throw std::runtime_error("Unable to initialize GLFW");
            }
        }, []() {
            glfwTerminate();
        });
        sendToUIThread(recursive_poll);
    }

    void terminate() {
        if (gui_pool->shutdown()) {
            gui_pool->join();
        }
        gui_pool->~pool_type();
    }

    void framebuffer_size_callback(GLFWwindow* glfw_window, int, int) {
        Window *window = reinterpret_cast<Window*> (glfwGetWindowUserPointer(glfw_window));
        window->size_changed = true;
    }

    void init_window(Window *window, WindowHints &hints) {

        window->glfw_window = runOnUIThreadAndWait([&]() {
            // Configure GLFW
            glfwDefaultWindowHints();

            for (const auto& pair : hints.int_hints) {
                glfwWindowHint(pair.first, pair.second);
            }

            for (const auto& pair : hints.string_hints) {
                glfwWindowHintString(pair.first, pair.second);
            }

            return glfwCreateWindow(hints.width,
                    hints.height,
                    hints.title.c_str(),
                    hints.monitor,
                    hints.share);
        });

        if (!window->glfwWindow()) {
            throw std::runtime_error("Failed to create the GLFW window");
        }

        glfwSetWindowUserPointer(window->glfwWindow(), window);

        runOnUIThreadAndWait([&]() {
            glfwSetFramebufferSizeCallback(window->glfwWindow(),
                    framebuffer_size_callback);
        });

        // Make the OpenGL context current
        glfwMakeContextCurrent(window->glfwWindow());

        // Enable v-sync
        glfwSwapInterval(hints.swap_interval);

        gladLoadGL();
    }

    void loop_window(Window *window) {

        while (!glfwWindowShouldClose(window->glfwWindow())) {

            if (window->size_changed.exchange(false)) {
                int fb_width, fb_height;
                window->getFramebufferSize(&fb_width, &fb_height);
                window->renderer()->onChangeSize(fb_width, fb_height);
            }

            window->renderer()->onDraw();

            glfwSwapBuffers(window->glfwWindow());
        }
    }

    void run_window(Window *window, WindowHints &hints,
            std::promise<void> *inited_in) {
        try {
            init_window(window, hints);
            inited_in->set_value();
        } catch (...) {
            inited_in->set_exception(std::current_exception());
            return;
        }

        window->renderer()->onCreate(window);

        loop_window(window);

        window->renderer()->onDispose();
        glfwDestroyWindow(window->glfwWindow());
    }

    Window::Window(Renderer *r, WindowHints hints) :
    p_renderer(r), user_pointer(nullptr), size_changed(true),
    cursor_pos_callback(nullptr) {
        std::promise<void> inited_in;
        auto inited_out = inited_in.get_future();
        graphic_thread = std::thread(run_window, this, std::ref(hints), &inited_in);
        inited_out.get();
    }

    void cursor_pos_callback_h(GLFWwindow* glfw_window,
            double xpos, double ypos) {
        Window *window = reinterpret_cast<Window*> (glfwGetWindowUserPointer(glfw_window));
        window->cursor_pos_callback(xpos, ypos);
    }

    cursor_pos_callback_type
    Window::setCursorPositionCallback(cursor_pos_callback_type callback) {
        return runOnUIThreadAndWait([&]() {
            cursor_pos_callback_type tmp = cursor_pos_callback;
            glfwSetCursorPosCallback(glfw_window,
                    callback ? cursor_pos_callback_h : nullptr);
            cursor_pos_callback = callback;
            return tmp;
        });
    }
}