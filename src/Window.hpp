#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <thread>
#include <future>
#include <atomic>
#include <functional>
#include <map>
#include "ThreadPool.hpp"
#include "config.hpp"

namespace gl_utils {

    void init();
    void terminate();

    struct Window;

    struct Renderer {

        virtual void onCreate(Window* /*window*/) { }

        virtual void onDraw() { }

        virtual void onChangeSize(int /*width*/, int /*height*/) { }

        virtual void onDispose() { }
    };

    struct EmptyRenderer : public Renderer {

        virtual void onDraw() override {
            glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        virtual void onChangeSize(int width, int height) override {
            glViewport(0, 0, width, height);
        }
    };

    using pool_type = JIO::thread_pool;
    extern pool_type* gui_pool;

    inline void sendToUIThread(std::function<void() > work) {
        gui_pool->send(work);
    }

    template<typename F>
    auto runOnUIThreadAndWait(F&& work) {
        return gui_pool->runAndWait(work);
    }

    struct WindowHints {
        int width, height;
        std::string title;
        GLFWmonitor *monitor;
        GLFWwindow *share;
        int swap_interval;
        std::map<int, int> int_hints;
        std::map<int, const char*> string_hints;

        void setTitle(std::string t) {
            title = t;
        }

        void setSize(int w, int h) {
            width = w;
            height = h;
        }

        void setMonitor(GLFWmonitor *m) {
            monitor = m;
        }

        void setShare(GLFWwindow *s) {
            share = s;
        }

        void setSwapInterval(int value) {
            swap_interval = value;
        }

        void windowHint(int hint, int value) {
            int_hints[hint] = value;
        }

        void windowStringHint(int hint, const char *value) {
            string_hints[hint] = value;
        }
    };

    inline WindowHints defaultWindowHints() {
        WindowHints out;
        out.setSize(1, 1);
        out.setTitle("");
        out.setMonitor(nullptr);
        out.setShare(nullptr);
        out.setSwapInterval(1);

        out.windowHint(GLFW_VISIBLE, GLFW_FALSE);
        out.windowHint(GLFW_RESIZABLE, GLFW_TRUE);
        return out;
    }

    using cursor_pos_callback_type = std::function<void(double, double) >;

    struct Window {
    private:
        GLFWwindow *glfw_window;
        Renderer *p_renderer;
        std::thread graphic_thread;
        std::atomic<void*> user_pointer;
        std::atomic<bool> size_changed;
        cursor_pos_callback_type cursor_pos_callback;

        friend void init_window(Window*, WindowHints &hints);
        friend void loop_window(Window*);
        friend void framebuffer_size_callback(GLFWwindow*, int, int);
        friend void cursor_pos_callback_h(GLFWwindow*, double, double);

    public:
        Window(Renderer *r, WindowHints hints);

        Window(Renderer *r) : Window(r, defaultWindowHints()) { }

        GLFWwindow* glfwWindow() {
            return glfw_window;
        }

        Renderer* renderer() {
            return p_renderer;
        }

        void* setUserPointer(void *ptr) {
            return user_pointer.exchange(ptr);
        }

        void* getUserPointer() {
            return user_pointer;
        }

        void setVisible(bool visible) {
            runOnUIThreadAndWait([visible, this]() {
                if (visible) {
                    glfwShowWindow(glfw_window);
                } else {
                    glfwHideWindow(glfw_window);
                }
            });
        }

        void setLocation(int xpos, int ypos) {
            runOnUIThreadAndWait([xpos, ypos, this]() {
                glfwSetWindowPos(glfw_window, xpos, ypos);
            });
        }

        void getLocation(int *xpos, int *ypos) {
            runOnUIThreadAndWait([&]() {
                glfwGetWindowPos(glfw_window, xpos, ypos);
            });
        }

        void setSize(int width, int height) {
            runOnUIThreadAndWait([width, height, this]() {
                glfwSetWindowSize(glfw_window, width, height);
            });
        }

        void getSize(int *width, int *height) {
            runOnUIThreadAndWait([&]() {
                glfwGetWindowSize(glfw_window, width, height);
            });
        }

        cursor_pos_callback_type
        setCursorPositionCallback(cursor_pos_callback_type);

        void getFramebufferSize(int *width, int *height) {
            runOnUIThreadAndWait([&]() {
                glfwGetFramebufferSize(glfw_window, width, height);
            });
        }

        void setTitle(std::string title) {
            runOnUIThreadAndWait([title, this]() {
                glfwSetWindowTitle(glfw_window, title.c_str());
            });
        }

        void waitClose() {
            graphic_thread.join();
        }
    };
}

#endif /* WINDOW_HPP */

