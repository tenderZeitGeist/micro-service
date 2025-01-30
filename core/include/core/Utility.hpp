//
// Created by zeitgeist on 23.09.24.
//

#pragma once

#include <core/logger/Logging.hpp>

#include <chrono>

namespace core::utility {
class ScopedTimer {
    using clock = std::chrono::high_resolution_clock;

public:
    explicit ScopedTimer(std::string message)
        : m_message(std::move(message)) {}

    ~ScopedTimer() {
        std::stringstream stream;
        const auto now = clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start);
        stream << m_message << " took " << duration.count() << " milliseconds";
        core::logger::log(stream.str());
    }

private:
    clock::time_point m_start{clock::now()};
    std::string m_message;
};


#if defined(ENABLE_PERFORMANCE_LOGGING)
#define MEASURE_PERFORMANCE(name, expr) ({core::utility::ScopedTimer t(name); expr;})
#else
#define MEASURE_PERFORMANCE(name, expr) (expr)
#endif

}
