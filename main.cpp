#include <iostream>

#include <rest/Alias.hpp>
#include <rest/Controller.hpp>
#include <rest/Connection.hpp>

constexpr std::size_t kMaxThreads = 4;

int main() {
    const auto numOfThreads = std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()), kMaxThreads);
    rest::Connection connection(net::ip::make_address(rest::kDefaultAddress), rest::kDefaultPort, numOfThreads);
    const auto controller = connection.initializeController({
        {
            rest::Method::get,
            rest::Endpoint("/compounds"),
            [](const rest::HttpRequest&) { return rest::Response(rest::http::status::ok, "Foo"); }
        },
        {
            rest::Method::get,
            rest::Endpoint("/compounds/{name}"),
            [](const rest::HttpRequest&) { return rest::Response(rest::http::status::ok, "Bar"); }
        },
        {
            rest::Method::get,
            rest::Endpoint("/animals/{name}"),
            [](const rest::HttpRequest&) { return rest::Response(rest::http::status::ok, "Bar"); }
        },
        {
            rest::Method::post,
            rest::Endpoint("compounds/{name}/animal/{name,species,age}"),
            [](const rest::HttpRequest&) { return rest::Response(rest::http::status::ok, "Bar"); }
        },
        {
            rest::Method::delete_,
            rest::Endpoint("compounds/{name}/animal/{name}"),
            [](const rest::HttpRequest&) { return rest::Response(rest::http::status::ok, "Bar"); }
        },
        {
            rest::Method::get,
            rest::Endpoint("/animal/{species}"),
            [](const rest::HttpRequest&) { return rest::Response(rest::http::status::ok, "Bar"); }
        },
    });
    connection.run();
    return 0;
}
