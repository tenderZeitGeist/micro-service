//
// Created by zeitgeist on 09.04.24.
//
#pragma once

#include "Alias.hpp"

namespace rest {

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

}