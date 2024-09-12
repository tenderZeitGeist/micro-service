#pragma once

#include "LoggerInterface.hpp"

namespace core::logger {

class LoggerImpl : public LoggerInterface {
    void log(const std::string& message) const override;
};

}
