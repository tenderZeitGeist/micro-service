//
// Created by abentler on 03.04.24.
//
#pragma once

#include "Request.hpp"
#include "Response.hpp"

#include <boost/regex.hpp>
#include <boost/url.hpp>

#include <functional>

namespace rest {

    class Controller {

        using Handler = std::function<Response(const Request&)>;

        struct Route {
            Route(http::verb method, boost::regex endpoint, Handler callback)
                : method(method)
                , endpoint(std::move(endpoint))
                , callback(std::move(callback))
            {}

            http::verb method;
            boost::regex endpoint;
            Handler callback;
        };

    public:

        enum class Method {
            GET,
            PUT,
            POST,
            DELETE,
        };

        template<typename F, Method M, typename... Args>
        bool emplaceRoute(boost::regex endpoint, F handler, Args... args) {
            if (std::ranges::find_if(m_routes, [&endpoint](const auto& route) { return endpoint == route.endpoint; })) {
                return false;
            }

            Handler callback = [handler = std::move(handler), endpoint, args..., this](const Request& request) {
                return handler(methodToVerb<M>(), request, endpoint, args...);
            };

            return true;
        }

        [[nodiscard]]
        Response execute(const Request& request) const {
            namespace url = boost::urls;

            url::result<url::url_view> target = url::parse_origin_form(request.target());
            if(target.has_error()) {
                return {http::status::bad_request, "Invalid target", std::string(rest::kTextPlain)};
            }

            const auto result = std::ranges::find_if(m_routes, [&request, &target](const auto& route){
                return route.method == request.method() && boost::regex_match(target->path(), route.endpoint);
            });

            if(result != m_routes.end()) {
                result->callback(request);
            }

            return {http::status::not_found, target->path() + " not found", std::string(rest::kTextPlain)};
        }

    private:
        template<Method M>
        http::verb methodToVerb() {
            switch(M) {
                case Method::GET:
                    return http::verb::get;
                case Method::PUT:
                    return http::verb::put;
                case Method::POST:
                    return http::verb::post;
                case Method::DELETE:
                    return http::verb::delete_;
            }
            static_assert(false);
        }

        std::vector<Route> m_routes;
    };

}
