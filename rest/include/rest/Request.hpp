//
// Created by abentler on 04.04.24.
//

#pragma once

#include "Alias.hpp"

#include <boost/beast/http.hpp>

namespace rest {

using Request = http::request<http::string_body>;
using Status = http::status;

}