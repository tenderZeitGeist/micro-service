//
// Created by abentler on 16.04.24.
//

#include <rest/Controller.hpp>

#include <core/service/ServiceControllerInterface.hpp>

#include <boost/url.hpp>

namespace rest {

Controller::Controller(std::unique_ptr<core::repository::ServiceControllerInterface> serviceController)
    : m_serviceController(std::move(serviceController)) {
    std::vector<Route> routes {
        { Method::get, routes::getCompounds, [this](const HttpRequest&) { return m_serviceController->getAllCompounds(); }},
        { Method::get, routes::getCompoundByName, [this](const HttpRequest& req) { return m_serviceController->getCompoundByName(req); }},
        { Method::get, routes::getAnimalByName, [this](const HttpRequest& req) { return m_serviceController->getAnimalByName(req); }},
        { Method::post, routes::postAnimalByCompound, [this](const HttpRequest& req) { return m_serviceController->addAnimalToCompound(req); }},
        { Method::delete_, routes::deleteAnimalByCompound, [this](const HttpRequest& req) { return m_serviceController->deleteAnimalFromCompound(req); }},
        { Method::get, routes::getAnimalsBySpecies, [this](const HttpRequest& req) { return m_serviceController->getAllAnimalsBySpecies(req); }}
    };
    for(auto& route : routes) {
        if(!emplaceRoute(std::move(route))) {
            std::abort();
        }
    }
}

bool Controller::emplaceRoute(Route route) {
    if (std::ranges::any_of(m_routes, [&rhs = route](const auto& lhs) { return lhs == rhs; })) {
        return false;
    }
    m_routes.emplace_back(std::move(route));
    return true;
}

[[nodiscard]] Response Controller::execute(const HttpRequest& request) const {
    namespace url = boost::urls;

    url::result<url::url_view> target = url::parse_origin_form(request.target());
    if (target.has_error()) {
        return {
            http::status::bad_request,
            "Invalid target",
            kTextPlain.data()
        };
    }

    const auto result = std::ranges::find_if(
        m_routes,
        [&request, &target](const auto& route) {
            return route.method == request.method() && boost::regex_match(target->path(), route.endpoint);
        }
    );

    if (result != m_routes.end()) {
        return result->callback(request);
    }

    return {
        http::status::not_found,
        target->path() + " not found",
        kTextPlain.data()
    };
}
}
