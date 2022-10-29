#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <thread>
#include <future>
#include <atomic>
#include <chrono>

#include <BlockingCollection.h>

namespace JIO {

    enum struct thread_pool_state {
        NOT_STARTED = 1,
        OK,
        FINISHED_OK,
        FINISHED_ERR,
        TERMINATING
    };

    class thread_pool {
    private:
        using queue_type = code_machina::BlockingQueue<std::function<void()>>;
        queue_type work_queue;
        std::thread worker;
        std::atomic<thread_pool_state> state;
        std::exception_ptr err;

        template<typename function_t>
        using function_ret_t = decltype(std::declval<function_t>()());

        thread_pool(const thread_pool&) = delete;
        thread_pool& operator=(const thread_pool&) = delete;
    public:

        template<typename F1, typename F2>
        thread_pool(size_t capacity, F1 start, F2 end) : work_queue(capacity),
        state(thread_pool_state::NOT_STARTED) {
            std::promise<void> inited_in;
            auto inited_out = inited_in.get_future();
            worker = std::thread([&]() {
                try {
                    start();
                } catch (...) {
                    inited_in.set_exception(std::current_exception());
                    return;
                }
                inited_in.set_value();
                try {
                    state = thread_pool_state::OK;
                    for (auto func : work_queue) {
                        try {
                            func();
                        } catch (...) {
                            //ignore
                        }
                    }
                } catch (...) {
                    err = std::current_exception();
                    state = thread_pool_state::FINISHED_ERR;
                    end();
                    return;
                }
                state = thread_pool_state::FINISHED_OK;
                end();
            });
            inited_out.get();
        }

        thread_pool(size_t capacity) : thread_pool(capacity, []() {
        }, []() {
        }) { }

        bool isOK() {
            return state == thread_pool_state::OK;
        }

        bool isFinished() {
            thread_pool_state tmp = state;
            return tmp == thread_pool_state::FINISHED_OK ||
            tmp == thread_pool_state::FINISHED_ERR;
        }

        void join() {
            worker.join();
        }

        bool shutdown() {
            thread_pool_state tmp = thread_pool_state::OK;
            bool swap = state.compare_exchange_strong(tmp,
            thread_pool_state::TERMINATING);
            if (swap) {
                work_queue.complete_adding();
                return true;
            }
            if (tmp == thread_pool_state::TERMINATING) {
                return true;
            }
            if (tmp == thread_pool_state::FINISHED_OK ||
            tmp == thread_pool_state::FINISHED_ERR) {
                return false;
            }
            throw std::runtime_error("Cannot shutdown");
        }

        void send(std::function<void() > work) {
            thread_pool_state tmp = state;
            if (tmp == thread_pool_state::OK) {
                work_queue.add(work);
                return;
            }
            if (tmp == thread_pool_state::FINISHED_ERR) {
                throw err;
            }
            throw std::runtime_error("Sending error");
        }

        template<typename F, typename R = function_ret_t<F>, typename... Tp>
        std::enable_if_t<!std::is_same<R, void>(), R>
        run(F work, Tp... args) {
            std::promise<R> data_in;
            auto data_out = data_in.get_future();
            send([&data_in, &work, &args...]() {
                try {
                    data_in.set_value(work());
                } catch (...) {
                    data_in.set_exception(std::current_exception());
                }
            });
            return data_out.get();
        }

        template<typename F, typename R = function_ret_t<F>, typename... Tp>
        std::enable_if_t<(std::is_same<R, void>()), R>
        run(F work, Tp... args) {
            std::promise<R> data_in;
            auto data_out = data_in.get_future();
            send([&data_in, &work, &args...]() {
                try {
                    work();
                    data_in.set_value();
                } catch (...) {
                    data_in.set_exception(std::current_exception());
                }
            });
            data_out.get();
        }
    };
}

#endif /* THREADPOOL_HPP */

