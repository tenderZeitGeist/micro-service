//
// Created by zeitgeist on 05.04.24.
//

#pragma once

#include <boost/beast/http.hpp>
#include <boost/regex.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace rest {
    /// forward declaration due to Handler alias
    struct Response;

    /// MIME types
    constexpr std::string_view kJson      = "application/json";
    constexpr std::string_view kTextPlain = "text/plain";

    /// Server info
    constexpr std::string_view kServerInfo = "Zoo Micro Service v0";

    /// Server config
    constexpr std::string_view kDefaultAddress = "0.0.0.0";
    constexpr uint16_t kDefaultPort = 8080;

    /// namespaces
    namespace beast = boost::beast;
    namespace http = beast::http;

    /// `using` aliases
    using Tcp = boost::asio::ip::tcp;
    using HttpResponse = http::response<http::string_body>;
    using HttpSerializer = http::response_serializer<http::string_body>;
    using HttpRequest = http::request<http::string_body>;
    using HttpParser = http::request_parser<http::string_body>;
    using Method = http::verb;
    using Endpoint = boost::regex;
    using Handler = std::function<Response(const HttpRequest&)>;

    namespace routes {
        static const Endpoint getCompounds("^/compounds$");
        static const Endpoint getCompoundByName("^/compounds/([a-zA-Z]{1,255})$");
        static const Endpoint getAnimalByName("^/animals/([a-zA-Z]{1,255})$");
        static const Endpoint postAnimalByCompound("^/compounds/([a-zA-Z]{1,255})/animals$");
        static const Endpoint deleteAnimalByCompound("^/compounds/([a-zA-Z]{1,255})/animals/([a-zA-Z]{1,255})$");
        static const Endpoint getAnimalsBySpecies("^/animals/species/(?i:(mammal|fish|bird|reptile))$");
    }
}
