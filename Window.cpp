#include "Window.hpp"
#include <thread>
#include <future>
#include <atomic>
#include "config.hpp"

#include <BlockingCollection.h>

using namespace gl_utils;

using queue_type = code_machina::BlockingQueue<std::function<void()>>;
queue_type gui_queue(16);
std::function<void() > poll;

std::promise<void> gui_finished_p;
std::future<void> gui_finished_f;

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error code: %i, desc: %s\n", error, description);
}

void gl_utils::init() {
    std::promise<void> inited;
    auto future = inited.get_future();

    gui_finished_p = std::promise<void>();
    gui_finished_f = gui_finished_p.get_future();

    std::thread gui_thread([&inited]() {
        glfwSetErrorCallback(error_callback);

        if (!glfwInit()) {
            std::exception_ptr ex = std::make_exception_ptr(
                    std::runtime_error("Unable to initialize GLFW"));
                    inited.set_exception(ex);
                    gui_finished_p.set_exception(ex);
            return;
        }
        inited.set_value();

        for (auto func : gui_queue) {
            func();
        }

        glfwTerminate();
        gui_finished_p.set_value();
    });
    future.get();
    gui_thread.detach();

    poll = []() {
        glfwPollEvents();
        if (!gui_queue.is_adding_completed()) {
            runOnUIThread(poll);
        }
    };
    runOnUIThread(poll);
}

void gl_utils::terminate() {
    gui_queue.complete_adding();
    gui_finished_f.get();
}

void gl_utils::runOnUIThread(std::function<void() > work) {
    gui_queue.add(work);
}

void key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
    Window *window = reinterpret_cast<Window*> (glfwGetWindowUserPointer(glfw_window));
    window->renderer->onKeyEvent(key, scancode, action, mods);
}

void cursor_pos_callback(GLFWwindow* glfw_window, double xpos, double ypos) {
    Window *window = reinterpret_cast<Window*> (glfwGetWindowUserPointer(glfw_window));
    window->renderer->onChangeCursorPos(xpos, ypos);
}

void mouse_button_callback(GLFWwindow* glfw_window, int button, int action, int mods) {
    Window *window = reinterpret_cast<Window*> (glfwGetWindowUserPointer(glfw_window));
    window->renderer->onMouseButtonEvent(button, action, mods);
}

void scroll_callback(GLFWwindow* glfw_window, double xoffset, double yoffset) {
    Window *window = reinterpret_cast<Window*> (glfwGetWindowUserPointer(glfw_window));
    window->renderer->onScroll(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* glfw_window, int width, int height) {
    Window *window = reinterpret_cast<Window*> (glfwGetWindowUserPointer(glfw_window));
    window->renderer->onChangeSize(width, height);
}

void init_window(Window *window, const char *title) {

    window->glfw_window = runOnUIThreadAndWait([&title]() {
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

    if (!window->glfw_window) {
        throw std::runtime_error("Failed to create the GLFW window");
    }

    glfwSetWindowUserPointer(window->glfw_window, window);

    runOnUIThreadAndWait([&window]() {
        //TODO
        glfwSetKeyCallback(window->glfw_window, key_callback);
        glfwSetCursorPosCallback(window->glfw_window, cursor_pos_callback);
        glfwSetMouseButtonCallback(window->glfw_window, mouse_button_callback);
        glfwSetScrollCallback(window->glfw_window, scroll_callback);
        glfwSetFramebufferSizeCallback(window->glfw_window, framebuffer_size_callback);
    });

    // Make the OpenGL context current
    glfwMakeContextCurrent(window->glfw_window);

    // Enable v-sync
    glfwSwapInterval(1);

    gladLoadGL();
}

void loop_window(Window *window) {
    while (!glfwWindowShouldClose(window->glfw_window)) {
        window->renderer->onDraw();

        glfwSwapBuffers(window->glfw_window);
    }
}

void run_window(Window *window, const char *title, std::promise<void> *p) {
    try {
        init_window(window, title);
        p->set_value();
    } catch (...) {
        p->set_exception(std::current_exception());
        return;
    }
    window->renderer->onCreate(window);
    loop_window(window);
    window->renderer->onDispose();
    glfwDestroyWindow(window->glfw_window);
}

gl_utils::Window::Window(Renderer *r, const char *title) : renderer(r) {
    std::promise<void> p;
    std::future<void> f = p.get_future();
    graphic_thread = std::thread(run_window, this, title, &p);
    f.get();
}