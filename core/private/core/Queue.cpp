#include "Queue.hpp"

#include <iostream>

namespace {
    void log(const std::string &message) {
        std::cout << message << '\n';
    }
}

namespace core::logger {
    Queue::Queue() {
    }

    Queue::~Queue() {
        stop();
    }

    void Queue::start() {
        m_running = true;
        m_thread = std::thread([this]{ process(); });
    }

    void Queue::stop() {
        {
            std::scoped_lock lk(m_mutex);
            m_running = false;
        }
        m_cv.notify_one();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    bool Queue::running() const {
        return m_running;
    }

    bool Queue::isEmpty() const {
        return m_queue.empty();
    }

    void Queue::queue(std::string&& message) {
        {
            std::scoped_lock lk(m_mutex);
            m_queue.push(std::move(message));
        }
        m_cv.notify_one();
    }

    void Queue::process() {
        std::unique_lock lk(m_mutex);
        while (m_running) {

            m_cv.wait(lk, [this]() { return !m_queue.empty() || !m_running; });
            lk.unlock();
            if (!m_running) {
                return;
            }

            while (!m_queue.empty()) {
                lk.lock();
                const auto message = m_queue.front();
                m_queue.pop();
                lk.unlock();
                log(message);
            }
        }
    }
}
