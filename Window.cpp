#include "Window.hpp"
#include <thread>
#include <future>
#include <atomic>
#include <chrono>
#include "config.hpp"

#include <BlockingCollection.h>

using namespace gl_utils;

using queue_type = code_machina::BlockingQueue<std::function<void()>>;

queue_type gui_queue;
std::function<void() > poll;

std::promise<void> gui_finished_in;
std::future<void> gui_finished_out;

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error code: %i, desc: %s\n", error, description);
}

void gl_utils::init() {
    new(&gui_queue) queue_type(16);

    std::promise<void> inited_in;
    auto inited_out = inited_in.get_future();

    gui_finished_in = std::promise<void>();
    gui_finished_out = gui_finished_in.get_future();

    std::thread gui_thread([&inited_in]() {
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
            for (auto func : gui_queue) {
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

    poll = []() {
        glfwPollEvents();
        if (!gui_queue.is_adding_completed()) {
            using namespace std::chrono_literals;
            //TODO
            std::this_thread::sleep_for(10ms);
            runOnUIThread(poll);
        }
    };
    runOnUIThread(poll);
}

void gl_utils::terminate() {
    gui_queue.complete_adding();
    gui_finished_out.get();
}

void gl_utils::runOnUIThread(std::function<void() > work) {
    gui_queue.add(work);
}

void framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height) {
    Window *window = reinterpret_cast<Window*> (glfwGetWindowUserPointer(glfw_window));
    window->renderer()->onChangeSize(width, height);
}

void init_window(Window *window, GLFWwindow **glfw_window, const char *title) {

    *glfw_window = runOnUIThreadAndWait([&title]() {
        //TODO
        // Configure GLFW
        glfwDefaultWindowHints();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // the window will stay hidden after creation
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // the window will be resizable

        return glfwCreateWindow(100, 10, title, nullptr, nullptr);
    });

    if (!window->glfwWindow()) {
        throw std::runtime_error("Failed to create the GLFW window");
    }

    glfwSetWindowUserPointer(window->glfwWindow(), window);

    runOnUIThreadAndWait([&window]() {
        //TODO
        /*glfwSetKeyCallback(window->glfw_window, key_callback);
        glfwSetCursorPosCallback(window->glfw_window, cursor_pos_callback);
        glfwSetMouseButtonCallback(window->glfw_window, mouse_button_callback);
        glfwSetScrollCallback(window->glfw_window, scroll_callback);*/
        glfwSetFramebufferSizeCallback(window->glfwWindow(), framebuffer_size_callback);
    });

    // Make the OpenGL context current
    glfwMakeContextCurrent(window->glfwWindow());

    // Enable v-sync
    glfwSwapInterval(1);

    gladLoadGL();
}

void loop_window(Window *window) {

    while (!glfwWindowShouldClose(window->glfwWindow())) {

        window->renderer()->onDraw();

        glfwSwapBuffers(window->glfwWindow());
    }
}

void run_window(Window *window, GLFWwindow **glfw_window, const char *title, std::promise<void> *p) {
    try {
        init_window(window, glfw_window, title);
        p->set_value();
    } catch (...) {
        p->set_exception(std::current_exception());
        return;
    }

    window->renderer()->onCreate(window);

    int fb_width, fb_height;
    window->getFramebufferSize(&fb_width, &fb_height);
    window->renderer()->onChangeSize(fb_width, fb_height);

    loop_window(window);

    window->renderer()->onDispose();
    glfwDestroyWindow(window->glfwWindow());
}

gl_utils::Window::Window(Renderer *r, const char *title) : p_renderer(r) {
    std::promise<void> p;
    std::future<void> f = p.get_future();
    graphic_thread = std::thread(run_window, this, &glfw_window, title, &p);
    f.get();
}