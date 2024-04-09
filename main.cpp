#include <iostream>

#include <rest/Alias.hpp>
#include <rest/Controller.hpp>
#include <rest/Connection.hpp>
#include <rest/Route.hpp>

int main() {
    auto address = net::ip::make_address(rest::kDefaultAddress);
    const auto numOfThreads = std::thread::hardware_concurrency();
    rest::Connection connection(address, rest::kDefaultPort, numOfThreads);
    const auto controller = connection.initializeController({
        {
            rest::Method::get,
            rest::Endpoint("/compound"),
            [](const rest::HttpRequest&) { return rest::Response(rest::http::status::ok, "Foo"); }
        },
        {
            rest::Method::get,
            rest::Endpoint("/zoo"),
            [](const rest::HttpRequest&) { return rest::Response(rest::http::status::ok, "Bar"); }
        }
    });
    connection.run();
    return 0;
}
