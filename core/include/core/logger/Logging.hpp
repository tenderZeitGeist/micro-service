#pragma once

#include <string>
#include <string_view>

namespace core::logger {

void log(const std::string& message);
void log(std::string_view message);
void log(const char* message);
void start();
void stop();

}
