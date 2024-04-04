//
// Created by zeitgeist on 05.04.24.
//

#pragma once

#include <boost/beast/http.hpp>

namespace rest {

    constexpr std::string_view kJson      = "application/json";
    constexpr std::string_view kTextPlain = "text/plain";
    namespace http = boost::beast::http;

}
