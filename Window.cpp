#include "Window.hpp"
#include <thread>
#include <future>
#include <atomic>
#include <chrono>
#include "config.hpp"

#include <BlockingCollection.h>

namespace gl_utils {

    //TODO
    //glfwSetKeyCallback(window, key_callback);
    //glfwSetCursorPosCallback(window, cursor_pos_callback);
    //glfwSetMouseButtonCallback(window, mouse_button_callback);
    //glfwSetScrollCallback(window, scroll_callback);

    using queue_type = code_machina::BlockingQueue<std::function<void()>>;

    queue_type* gui_queue;

    std::promise<void> gui_finished_in;
    std::future<void> gui_finished_out;

    std::function<void() > recursive_poll;

    void error_callback(int error, const char* description) {
        fprintf(stderr, "Error code: %i, desc: %s\n", error, description);
    }

    void init() {
        gui_queue = new queue_type(16);

        std::promise<void> inited_in;
        auto inited_out = inited_in.get_future();

        gui_finished_in = std::promise<void>();
        gui_finished_out = gui_finished_in.get_future();

        std::thread gui_thread([&]() {
            glfwSetErrorCallback(error_callback);

            if (!glfwInit()) {
                std::exception_ptr ex = std::make_exception_ptr(
                        std::runtime_error("Unable to initialize GLFW"));
                        inited_in.set_exception(ex);
                        gui_finished_in.set_exception(ex);
                return;
            }
            inited_in.set_value();

            try {
                for (auto func : *gui_queue) {
                    func();
                }
            } catch (...) {
                gui_finished_in.set_exception(std::current_exception());
                        glfwTerminate();
                return;
            }

            glfwTerminate();
            gui_finished_in.set_value();
        });
        inited_out.get();
        gui_thread.detach();

        recursive_poll = []() {
            glfwPollEvents();
            if (!gui_queue->is_adding_completed()) {
                using namespace std::chrono_literals;
                //TODO
                std::this_thread::sleep_for(10ms);
                runOnUIThread(recursive_poll);
            }
        };
        runOnUIThread(recursive_poll);
    }

    void terminate() {
        gui_queue->complete_adding();
        gui_finished_out.get();
        delete gui_queue;
    }

    void runOnUIThread(std::function<void() > work) {
        //TODO checks
        gui_queue->add(work);
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
            glfwSetFramebufferSizeCallback(window->glfwWindow(), framebuffer_size_callback);
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

    void run_window(Window *window, WindowHints &hints, std::promise<void> *inited_in) {
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

    Window::Window(Renderer *r, WindowHints hints) : p_renderer(r), size_changed(true) {
        std::promise<void> inited_in;
        auto inited_out = inited_in.get_future();
        graphic_thread = std::thread(run_window, this, std::ref(hints), &inited_in);
        inited_out.get();
    }
}