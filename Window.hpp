#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <thread>
#include <future>
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

    void runOnUIThread(std::function<void()>);

    template<typename lambda_t>
    using lambda_ret_t = decltype(std::declval<lambda_t>()());

    template<typename F, typename R = lambda_ret_t<F>, typename... Tp>
    std::enable_if_t<!std::is_same<R, void>(), R>
    runOnUIThreadAndWait(F work, Tp... args) {
        std::promise<R> data_in;
        auto data_out = data_in.get_future();
        runOnUIThread([&data_in, &work, &args...]() {
            try {
                data_in.set_value(work());
            } catch (...) {
                data_in.set_exception(std::current_exception());
            }
        });
        return data_out.get();
    }

    template<typename F, typename R = lambda_ret_t<F>, typename... Tp>
    std::enable_if_t<(std::is_same<R, void>()), R>
    runOnUIThreadAndWait(F work, Tp... args) {
        std::promise<R> data_in;
        auto data_out = data_in.get_future();
        runOnUIThread([&data_in, &work, &args...]() {
            try {
                work();
                data_in.set_value();
            } catch (...) {
                data_in.set_exception(std::current_exception());
            }
        });
        data_out.get();
    }

    struct Window {
        GLFWwindow *glfw_window;
        Renderer *renderer;
        std::thread graphic_thread;

        Window(Renderer *r, const char *title);

        Window(Renderer *r) : Window(r, "") { }

        void setVisible(bool visible) {
            runOnUIThreadAndWait([&visible, this]() {
                if (visible) {
                    glfwShowWindow(glfw_window);
                } else {
                    glfwHideWindow(glfw_window);
                }
            });
        }

        void setLocation(int xpos, int ypos) {
            runOnUIThreadAndWait([&xpos, &ypos, this]() {
                glfwSetWindowPos(glfw_window, xpos, ypos);
            });
        }

        void setDefaultLocation() {
            runOnUIThreadAndWait([this]() {
                const GLFWvidmode* vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

                if (!vidmode) {
                    throw std::runtime_error("Video mode is null");
                }

                int width, height;
                glfwGetWindowSize(glfw_window, &width, &height);

                // Center the window

                glfwSetWindowPos(glfw_window,
                        (vidmode->width - width) / 2,
                        (vidmode->height - height) / 2);
            });
        }

        void setSize(int width, int height) {
            runOnUIThreadAndWait([&width, &height, this]() {
                glfwSetWindowSize(glfw_window, width, height);
            });
        }

        void setTitle(std::string title) {
            runOnUIThreadAndWait([&title, this]() {
                glfwSetWindowTitle(glfw_window, title.c_str());
            });
        }

        void waitClose() {
            graphic_thread.join();
        }
    };
}

#endif /* WINDOW_HPP */

