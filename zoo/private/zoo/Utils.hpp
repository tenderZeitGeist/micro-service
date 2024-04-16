//
// Created by abentler on 16.04.24.
//

#pragma once

#include <boost/json.hpp>

namespace zoo {
    class Animal;
    class Compound;
}

namespace hateos {

namespace json = boost::json;

    namespace animals {
        json::array makeHypermediaLinks(const std::string& compoundId, const std::string& animalId, const std::string& animalSpecies);
        json::object toJson(const std::string& compoundId, std::reference_wrapper<const zoo::Animal> animalRef);
        json::object toJson(const std::string& compoundId, const std::vector<std::reference_wrapper<const zoo::Animal>>& animalRefs);
    }

    namespace compounds {
        json::array makeHypermediaLinks(const std::string& compoundId);
        json::object toJson(std::reference_wrapper<const zoo::Compound> compoundRef, const std::vector<std::reference_wrapper<const zoo::Animal>>& animalRefs);
    }

}
