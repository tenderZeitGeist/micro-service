
#include "core/logger/Logging.hpp"
#include "core/LoggerImpl.hpp"
#include "core/MessageQueue.hpp"

#include <cassert>

namespace core::logger {

// Private

MessageQueue m_messageQueue{std::make_unique<LoggerImpl>()};

void log(std::string&& message) {
    if(!m_messageQueue.running()) {
        assert(false);
        return;
    }
    m_messageQueue.queue(std::move(message));
}

template<typename T>
concept LiteralConvertiable =
        std::is_convertible_v<std::remove_cvref_t<T>, std::string>
        || std::is_constructible_v<std::remove_cvref_t<T>, std::string>;
void forward(LiteralConvertiable auto&& message) {
    log(std::string(std::forward<decltype(message)>(message)));
}

// Public API

void log(const std::string& message) {
    if(!m_messageQueue.running()) {
        assert(false);
        return;
    }

    m_messageQueue.queue(message);
}

void log(std::string_view message) {
    forward(message);
}

void log(const char* message) {
    forward(message);
}

void start() {
    m_messageQueue.start();
}

void stop() {
    m_messageQueue.stop();
}

}
