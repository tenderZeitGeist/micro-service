#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace core::logger {

class LoggerInterface;

class MessageQueue {
public:
    explicit MessageQueue(std::unique_ptr<LoggerInterface> logger);
    ~MessageQueue();

    void start();
    void stop();
    [[nodiscard]] bool running() const;
    [[nodiscard]] bool isEmpty() const;
    void queue(std::string message);

private:
    void process();

    std::atomic_bool m_running;
    std::condition_variable m_cv;
    std::queue<std::string> m_queue;
    std::mutex m_mutex;
    std::thread m_thread;

    std::unique_ptr<LoggerInterface> m_logger;
};

}
