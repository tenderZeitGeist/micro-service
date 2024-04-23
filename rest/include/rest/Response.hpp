//
// Created by abentler on 04.04.24.
//
#pragma once

#include "Alias.hpp"

#include <boost/beast/http.hpp>

namespace rest {

struct Response {
    http::status status{http::status::not_implemented};
    std::string body;
    std::string_view mimeType{rest::kJson};
};

}