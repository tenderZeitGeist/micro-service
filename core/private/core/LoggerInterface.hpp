#pragma once

#include <string>

namespace core::logger {

class LoggerInterface {
public:
    virtual ~LoggerInterface() = default;
    virtual void log(const std::string& message) const = 0;
};

}
