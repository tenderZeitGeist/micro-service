//
// Created by abentler on 03.04.24.
//
#pragma once

#include "Alias.hpp"
#include "Response.hpp"

#include <boost/regex.hpp>

namespace core::repository {
    class ServiceControllerInterface;
}

namespace rest {

    class Controller {

        struct Route {
            Route(Method method, Endpoint endpoint, Handler callback)
                : method(method)
                , endpoint(std::move(endpoint))
                , callback(std::move(callback)) {}

            friend bool operator==(const Route& lhs, const Route& rhs) {
                return lhs.endpoint == rhs.endpoint && lhs.method == rhs.method;
            }

            Method method;
            Endpoint endpoint;
            Handler callback;
        };

    public:
        explicit Controller(std::unique_ptr<core::repository::ServiceControllerInterface> serviceController);
        [[nodiscard]] bool emplaceRoute(Route route);
        [[nodiscard]] Response execute(const HttpRequest& request) const;

    private:
        std::vector<Route> m_routes;
        std::unique_ptr<core::repository::ServiceControllerInterface> m_serviceController;
    };

}
