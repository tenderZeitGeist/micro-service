#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <queue>

namespace core::logger {

class Queue {
public:
    explicit Queue();
    ~Queue();

    void start();
    void stop();
    [[nodiscard]] bool running() const;
    [[nodiscard]] bool isEmpty() const;
    void queue(std::string&& message);

private:
    void process();

    std::atomic_bool m_running;
    std::condition_variable m_cv;
    std::queue<std::string> m_queue;
    std::mutex m_mutex;
    std::thread m_thread;
};

}
