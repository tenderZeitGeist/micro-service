//
// Created by zeitgeist on 05.04.24.
//

#pragma once

#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace rest {
    /// MIME types
    constexpr std::string_view kJson      = "application/json";
    constexpr std::string_view kTextPlain = "text/plain";

    /// Server info
    constexpr std::string_view kServerInfo = "Zoo Micro Service v0";

    /// namespaces
    namespace beast = boost::beast;
    namespace http = beast::http;

    /// `using` aliases
    using tcp = boost::asio::ip::tcp;
    using http_response = http::response<http::string_body>;
    using http_serializer = http::response_serializer<http::string_body>;
    using http_request = http::request<http::string_body>;
    using http_parser = http::request_parser<http::string_body>;
}
