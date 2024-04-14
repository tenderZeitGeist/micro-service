//
// Created by abentler on 03.04.24.
//
#pragma once

#include "Alias.hpp"
#include "Response.hpp"
#include "Route.hpp"

#include <boost/regex.hpp>
#include <boost/url.hpp>

#include <functional>

#include <iostream>

namespace rest {

    enum class Verb {
        GET,
        PUT,
        POST,
        DELETE,
    };

    class Controller {

        using Method = http::verb;

    public:

        bool emplaceRoute(Route route) {
            if(std::ranges::any_of(m_routes, [&rhs = route](const auto& lhs) { return lhs == rhs; })) {
                return false;
            }
            m_routes.emplace_back(std::move(route));
            return true;
        }

        [[nodiscard]]
        Response execute(const HttpRequest& request) const {
            namespace url = boost::urls;

            url::result<url::url_view> target = url::parse_origin_form(request.target());
            if(target.has_error()) {
                return {http::status::bad_request, "Invalid target", kTextPlain.data()};
            }

            std::cout << target << '\n';

            const auto result = std::ranges::find_if(m_routes, [&request, &target](const auto& route){
                return route.method == request.method() && boost::regex_match(target->path(), route.endpoint);
            });

            if(result != m_routes.end()) {
                return result->callback(request);
            }

            return {http::status::not_found, target->path() + " not found", kTextPlain.data()};
        }

    private:
        http::verb verbToMethod(Verb v) {
            switch(v) {
                case Verb::GET:
                    return Method::get;
                case Verb::PUT:
                    return http::verb::put;
                case Verb::POST:
                    return http::verb::post;
                case Verb::DELETE:
                    return http::verb::delete_;
            }
            assert(false);
        }

        std::vector<Route> m_routes;
    };

}
