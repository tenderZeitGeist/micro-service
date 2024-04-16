//
// Created by abentler on 13.04.24.
//

#pragma once

#include <rest/Response.hpp>
#include <rest/Request.hpp>

namespace core::repository {

class ServiceControllerInterface {
    public:
    virtual ~ServiceControllerInterface() = default;
    [[nodiscard]] virtual rest::Response getAllCompounds() const = 0;
    [[nodiscard]] virtual rest::Response getCompoundByName(const rest::Request& r) const = 0;
    [[nodiscard]] virtual rest::Response getAnimalByName(const rest::Request& r) const = 0;
    [[nodiscard]] virtual rest::Response addAnimalToCompound(const rest::Request& r) = 0;
    [[nodiscard]] virtual rest::Response deleteAnimalFromCompound(const rest::Request& r) = 0;
    [[nodiscard]] virtual rest::Response getAllAnimalsBySpecies(const rest::Request& r) = 0;
};

}
