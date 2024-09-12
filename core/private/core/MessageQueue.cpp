
#include "MessageQueue.hpp"
#include "LoggerInterface.hpp"

namespace core::logger {

MessageQueue::MessageQueue(std::unique_ptr<LoggerInterface> logger)
    : m_logger(std::move(logger)) {}

MessageQueue::~MessageQueue() {
    stop();
}

void MessageQueue::start() {
    m_running = true;
    m_thread = std::thread([this] { process(); });
}

void MessageQueue::stop() {
    {
        std::scoped_lock lk(m_mutex);
        m_running = false;
    }
    m_cv.notify_one();
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

bool MessageQueue::running() const {
    return m_running;
}

bool MessageQueue::isEmpty() const {
    return m_queue.empty();
}

void MessageQueue::queue(std::string message) {
    {
        std::scoped_lock lk(m_mutex);
        m_queue.push(std::move(message));
    }
    m_cv.notify_one();
}

void MessageQueue::process() {
    std::unique_lock lk(m_mutex);
    while (m_running) {
        m_cv.wait(lk, [this]() { return !m_queue.empty() || !m_running; });
        if(lk.owns_lock()) {
            lk.unlock();
        }

        if (!m_running) {
            return;
        }

        while (!m_queue.empty()) {
            lk.lock();
            const auto message = m_queue.front();
            m_queue.pop();
            lk.unlock();
            m_logger->log(message);
        }
    }
}
}
