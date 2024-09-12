#include "LoggerImpl.hpp"

#include <iostream>

namespace core::logger {

void LoggerImpl::log(const std::string& message) const {
    std::cout << message << '\n';
}

}
