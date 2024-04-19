//
// Created by abentler on 16.04.24.
//

#include "Utils.hpp"

#include <zoo/animal/Animal.hpp>
#include <zoo/compound/Compound.hpp>

namespace hateos {

namespace animals {

json::array makeHypermediaLinks(const std::string& compoundId, const std::string& animalId, const std::string& animalSpecies) {
    return {
        json::object{
            {"rel", "self"},
            {"href", +"/animals/" + animalId}
        },
        json::object{
            {"rel", "species"},
            {"href", "/animals/species/" + animalSpecies},
            {"title", "Retrieve all animals from that species"}
        },
        json::object{
            {"rel", "getCoumpound"},
            {"href", "/compounds/" + compoundId},
            {"title", "Get associated compound"}
        },
        json::object{
            {"rel", "delete"},
            {"href", "/compounds/" + compoundId + "/animals/" + animalId},
            {"title", "Deletes this species from the aforementioned compound"}
        }
    };
}

json::object toJson(const std::string& compoundId, std::reference_wrapper<const zoo::Animal> animalRef) {
    const auto& animal = animalRef.get();
    auto links = makeHypermediaLinks(compoundId, animal.getName(), animal.getSpeciesString());
    return json::object{
        {"id", animal.getId()},
        {"name", animal.getName()},
        {"age", animal.getAge()},
        {"species", animal.getSpeciesString()},
        {"_links", json::array(std::move(links))}
    };
}

json::object toJson(const std::string& compoundId, const std::vector<std::reference_wrapper<const zoo::Animal>>& animalRefs) {
    json::object object{{"animals", json::array()}};
    for (auto animalRef: animalRefs) {
        object["animals"].as_array().emplace_back(toJson(compoundId, animalRef));
    }
    return object;
}

}

namespace compounds {
json::array makeHypermediaLinks(const std::string& compoundId) {
    return json::array{
        json::object{
            {"rel", "self"},
            {"href", "/compounds/" + compoundId}
        },
        json::object{
            {"rel", "add"},
            {"href", "/compounds/" + compoundId + "/animals"},
            {"title", "Adds an animal to the compound"}
        },
        json::object{
            {"rel", "all"},
            {"href", "/compounds"}
        }
    };
}

json::object toJson(std::reference_wrapper<const zoo::Compound> compoundRef, const std::vector<std::reference_wrapper<const zoo::Animal>>& animalRefs) {
    const auto& compound = compoundRef.get();
    auto links = makeHypermediaLinks(compound.getName());
    json::object object{
        {"id", compound.getId()},
        {"name", compound.getName()},
        {"size", compound.getSize()},
        {"animals", json::array()},
        {"_links", std::move(links)}
    };

    for (auto animalRef: animalRefs) {
        object["animals"].as_array().emplace_back(animals::toJson(compound.getName(), animalRef));
    }

    return object;
}

}

}
