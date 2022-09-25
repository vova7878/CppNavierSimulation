#include <iostream>
#include <stdexcept>
#include <chrono>

#include "config.hpp"
#include "Renderer.hpp"

GLFWwindow* window;
Renderer renderer;

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error code: %i, desc: %s\n", error, description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    renderer.onKeyEvent(window, key, scancode, action, mods);
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    renderer.onChangeCursorPos(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    renderer.onMouseButtonEvent(window, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    renderer.onScroll(window, xoffset, yoffset);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    renderer.onChangeSize(window, width, height);
}

void init(int width, int height, bool resizable, const char* title) {
    // Setup an error callback.
    glfwSetErrorCallback(error_callback);

    // Initialize GLFW. Most GLFW functions will not work before doing this.
    if (!glfwInit()) {
        throw std::runtime_error("Unable to initialize GLFW");
    }

    // Configure GLFW
    glfwDefaultWindowHints(); // optional, the current window hints are already the default

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // the window will stay hidden after creation
    glfwWindowHint(GLFW_RESIZABLE, resizable); // the window will be resizable

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create the GLFW window");
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    // Get the resolution of the primary monitor
    const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    if (!vidmode) {
        glfwTerminate();
        throw std::runtime_error("Video mode is null");
    }

    // Center the window
    glfwSetWindowPos(window,
            (vidmode->width - width) / 2,
            (vidmode->height - height) / 2);

    // Make the OpenGL context current
    glfwMakeContextCurrent(window);

    // Enable v-sync
    glfwSwapInterval(1);

    // Make the window visible
    glfwShowWindow(window);

    gladLoadGL();
}

void dispose() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

char const* gl_error_string(GLenum const err) noexcept {
    switch (err) {
            // opengl 2 errors (8)
        case GL_NO_ERROR:
            return "GL_NO_ERROR";

        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";

        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";

        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";

        case GL_STACK_OVERFLOW:
            return "GL_STACK_OVERFLOW";

        case GL_STACK_UNDERFLOW:
            return "GL_STACK_UNDERFLOW";

        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        default:
            return "unknown error";
    }
}

void printErrors() {
    int err = glGetError();
    while (err) {
        std::cout << "errors: " << gl_error_string(err) << "\n";
        err = glGetError();
    }
}

int main(int, char**) {
    int w = 500, h = 500;

    init(w, h, true, "Simulation");
    printErrors();

    renderer.onChangeSize(window, w, h);
    printErrors();

    renderer.onCreate();
    printErrors();

    auto time = std::chrono::high_resolution_clock::now();
    int i = 0;

    while (!glfwWindowShouldClose(window)) {
        renderer.onDraw();
        printErrors();

        glfwSwapBuffers(window); // swap the color buffers

        // Poll for window events. The key callback above will only be
        // invoked during this call.
        glfwPollEvents();

        i++;
        auto tmp_time = std::chrono::high_resolution_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::nanoseconds>(tmp_time - time).count();
        if (delta > 100000000) {
            time = tmp_time;
            double fps = i * 1000000000.0 / delta;
            glfwSetWindowTitle(window, toString("Simulation ", fps).c_str());
            i = 0;
        }
    }

    renderer.onDispose();
    printErrors();

    dispose();

    return 0;
}

