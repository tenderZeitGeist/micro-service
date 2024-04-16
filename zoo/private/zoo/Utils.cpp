//
// Created by abentler on 16.04.24.
//

#include "Utils.hpp"

#include <zoo/animal/Animal.hpp>
#include <zoo/compound/Compound.hpp>

namespace hateos {

namespace animals {

json::array makeHypermediaLinks(const std::string& baseUri, const std::string& compoundId, const std::string& animalId, const std::string& animalSpecies) {
    return {
        json::object{
                { "rel", "self" },
                { "href", baseUri + "/" + compoundId + "/" + animalId }
        },
        json::object {
            { "rel", "all" },
            { "href", baseUri + "/animals/" + animalSpecies },
            {"title", "Retrieve all animals from that species"}
        },
        json::object {
            {"rel", "delete"},
            { "href", baseUri + "/" + compoundId + "/" + animalId },
            {"title", "Deletes this species from the aforementioned compound"}
        }
    };
}

json::object toJson(std::reference_wrapper<const zoo::Animal> animalRef) {
    const auto& animal = animalRef.get();
    auto links = makeHypermediaLinks("", "", animal.getName(), animal.getSpeciesString());
    return json::object{
        {"id", animal.getId()},
        {"name", animal.getName()},
        {"age", animal.getAge()},
        {"species", animal.getSpeciesString()},
        {"_links", json::array(std::move(links))}
        };
}

json::object toJson(const std::vector<std::reference_wrapper<const zoo::Animal>>& animalRefs) {
    json::object object{{"animals", json::array()}};
    for(auto animalRef : animalRefs) {
        object["animals"].as_array().emplace_back(toJson(animalRef));
    }
    return object;
}

}

namespace compounds {
json::array makeHypermediaLinks(const std::string& baseUri, const std::string& resourceId) {
    return json::array{
            json::object{
                { "rel", "self" },
                { "href", baseUri + "/" + resourceId }
            },
            json::object{
                { "rel", "add" },
                { "href", baseUri + "/" + resourceId  + "/animals"},
                {"title", "Adds an animal to the compound"}
            },
            json::object{
                { "rel", "all" },
                { "href", baseUri }
            }
    };
}

json::object toJson(const std::string& baseUri, std::reference_wrapper<const zoo::Compound> compoundRef, const std::vector<std::reference_wrapper<const zoo::Animal>>& animalRefs) {
    const auto& compound = compoundRef.get();
    auto links = makeHypermediaLinks(baseUri, compound.getName());
    json::object object {
        {"id", compound.getId()},
        {"name", compound.getName()},
        {"size", compound.getSize()},
        {"animals", json::array()},
        {"_links", std::move(links)}
    };

    for(auto animalRef : animalRefs) {
        object["animals"].as_array().emplace_back(animals::toJson(animalRef));
    }

    return object;
}

}

}
