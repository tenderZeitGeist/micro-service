//
// Created by abentler on 13.04.24.
//

#include "zoo/services/ServiceController.hpp"

#include "zoo/services/AnimalService.hpp"
#include "zoo/services/CompoundService.hpp"
#include "zoo/services/ServiceInterface.hpp"

#include <boost/regex.hpp>
#include <boost/json.hpp>

#include <iostream>

namespace json = boost::json;

namespace {

json::object makeHypermediaLinks(std::string_view baseUri, const std::string& resourceId) {
    return {
        { "_links", json::array{
            json::object{
                { "rel", "self" },
                { "href", std::string(baseUri) + "/" + resourceId }
            },
            json::object{
                { "rel", "add" },
                { "href", std::string(baseUri) + "/" + resourceId  + "/animals"},
                {"title", "Adds an animal to the compound"}
            },
            json::object{
                { "rel", "all" },
                { "href", baseUri }
            }
        }}
    };
}

json::object makeHypermediaLinks(
    std::string_view baseUri,
    const std::string& compoundId,
    const std::string& animalId,
    const std::string& animalSpecies) {
    const std::string base = std::string(baseUri);
    return {
        { "self", base + "/" + compoundId + "/" + animalId },
        { "species", base + "/animals/" + animalSpecies},
        { "delete", base + "/animals/"}
    };
}

json::object toJson(std::reference_wrapper<const zoo::Animal> animalRef) {
    const auto& animal = animalRef.get();
    return json::object{
        {"id", animal.getId()},
        {"name", animal.getName()},
        {"age", animal.getAge()},
        {"species", animal.getSpeciesString()}
        };
}

json::object toJson(const std::vector<std::reference_wrapper<const zoo::Animal>>& animalRefs) {
    json::object object{{"animals", json::array()}};
    for(auto animalRef : animalRefs) {
        object["animals"].as_array().emplace_back(toJson(animalRef));
    }
    return object;
}

json::object toJson(
    std::string_view target,
    std::reference_wrapper<const zoo::Compound> comoundRef,
    const std::vector<std::reference_wrapper<const zoo::Animal>>& animalRefs) {
    const auto& compound = comoundRef.get();
    auto links = makeHypermediaLinks(target, compound.getName());
    json::object object {
        {"id", compound.getId()},
        {"name", compound.getName()},
        {"size", compound.getSize()},
        {"animals", json::array()},
        {"_links", std::move(links)}
    };

    for(auto animalRef : animalRefs) {
        object["animals"].as_array().emplace_back(toJson(animalRef));
    }

    return object;
}

}

namespace zoo {

static const rest::Response kNotFoundResponse {
    rest::http::status::not_found,
    "The resource couldn't be located",
    rest::kTextPlain.data()
};

static const rest::Response kBadRequest {
    rest::http::status::bad_request,
    "The request contains insufficient or false data",
    rest::kTextPlain.data()
};

static const rest::Response kInternalError {
    rest::http::status::internal_server_error,
    "The request couldn't be processed because of an internal error",
    rest::kTextPlain.data()
};

ServiceController::ServiceController(std::unique_ptr<AnimalService> animalService, std::unique_ptr<CompoundService> compoundService)
    : m_animalService(std::move(animalService)),
      m_compoundService(std::move(compoundService)) {}

rest::Response ServiceController::getAllCompounds() const {
    const auto compounds = m_compoundService->getAllTargetEntities();
    auto body = parse("/compounds", compounds);
    if (body.empty()) {
        return kNotFoundResponse;
    }
    return rest::Response{
        rest::http::status::ok,
        std::move(body),
        rest::kJson.data()
    };
}

rest::Response ServiceController::getCompoundByName(const rest::Request& r) const {
    boost::cmatch matches;
    const auto& target = r.target();
    if(!boost::regex_search(target.begin(), target.end(), matches, rest::routes::getCompoundByName)
        || matches.size() != 2) {
        return kBadRequest;
    }
    const auto compound = m_compoundService->getEntityByName(matches[1].str());
    if (!compound) {
        return kNotFoundResponse;
    }
    std::string body = parse(r.target(), {*compound});
    return {
        rest::http::status::ok,
        std::move(body),
        rest::kJson.data()
    };
}

rest::Response ServiceController::getAnimalByName(const rest::Request& r) const {
    boost::cmatch matches;
    const auto& target = r.target();
    if(!boost::regex_search(target.begin(), target.end(), matches, rest::routes::getAnimalByName)
        || matches.size() != 2) {
        return kBadRequest;
    }
    const auto animal = m_animalService->getEntityByName(matches[1].str());
    if (!animal) {
        return kNotFoundResponse;
    }
    std::string body = json::serialize(json::array().emplace_back(toJson(*animal)));
    return {
        rest::http::status::ok,
        std::move(body),
        rest::kJson.data()
    };
}

rest::Response ServiceController::addAnimalToCompound(const rest::Request& r) {
    boost::cmatch matches;
    const auto& target = r.target();
    if(!boost::regex_search(target.begin(), target.end(), matches, rest::routes::postAnimalByCompound)
        || matches.size() != 2) {
        return kBadRequest;
    }

    const auto compound = m_compoundService->getEntityByName(matches[1].str());
    if (!compound) {
        return kNotFoundResponse;
    }

    const auto animalId = tryAdd(r.body());
    if(!animalId) {
        return kBadRequest;
    }

    if(!m_compoundService->addAnimal(*compound, *animalId)) {
        [[maybe_unused]] const auto _ = m_animalService->deleteAnimal(*animalId);
        return kBadRequest;
    }

    const auto animal = m_animalService->getEntityById(*animalId);
    if(!animal) {
        return kInternalError;
    }

    std::string body = json::serialize(json::array().emplace_back(toJson(*animal)));
    return {
        rest::http::status::ok,
        std::move(body),
        rest::kJson.data()
    };
}

rest::Response ServiceController::deleteAnimalFromCompound(const rest::Request& r) {
    const auto deleteResourceId = tryDelete(r.target());
    if(!deleteResourceId) {
        return kBadRequest;
    }
    const auto& [compoundName, animalName] = *deleteResourceId;
    const auto compoundRef = m_compoundService->getEntityByName(compoundName);
    if(!compoundRef) {
        return kBadRequest;
    }
    const auto animalRef = m_animalService->getEntityByName(animalName);
    if(!animalRef) {
        return kBadRequest;
    }

    const auto animalIds = compoundRef.value().get().getAnimals();
    const auto searchedId = animalRef.value().get().getId();

    if(!std::ranges::any_of(animalIds, [searchedId](std::size_t id) { return id == searchedId; })) {
        return kBadRequest;
    }

    const auto success = m_compoundService->deleteAnimal(*compoundRef, searchedId)
                          && m_animalService->deleteAnimal(searchedId);
    if(!success) {
        return kInternalError;
    }

    return {
        rest::http::status::no_content,
        ""
    };
}

rest::Response ServiceController::getAllAnimalsBySpecies(const rest::Request& r) {
    boost::cmatch matches;
    const auto& target = r.target();
    if(!boost::regex_search(target.begin(), target.end(), matches, rest::routes::getAnimalsBySpecies)
        || matches.size() != 2) {
        return kBadRequest;
    }

    const auto speciesString = matches[1].str();
    try {
        const auto animals = m_animalService->getAllTargetEntities();
        const auto species = Animal::stringToSpecies(speciesString);
        auto filteredView = animals
                | std::ranges::views::filter([species](auto animal) { return animal.get().getSpecies() == species; })
                | std::views::common;
        std::string body = json::serialize(json::array().emplace_back(toJson({filteredView.begin(), filteredView.end()})));
        return {
            rest::http::status::ok,
            std::move(body),
            rest::kJson.data()
        };
    } catch (const std::exception&) {
        return kBadRequest;
    }
}

std::string ServiceController::parse(std::string_view target, const std::vector<std::reference_wrapper<const Compound>>& compounds) const {
    json::object object{{"compounds", json::array()}};
    for (auto compoundRef: compounds) {
        const auto animalRefs = m_animalService->getAnimalsByIds(compoundRef.get().getAnimals());
        object["compounds"].as_array().emplace_back(toJson(target, compoundRef, animalRefs));
    }
    return json::serialize(json::array().emplace_back(std::move(object)));
}

std::optional<std::size_t> ServiceController::tryAdd(std::string_view body) const {
    try {
        const auto json = json::parse(body);
        auto name = std::string(json.at("name").as_string());
        const auto age = static_cast<std::size_t>(json.at("age").as_int64());
        auto species = std::string(json.at("species").as_string());
        auto animal = std::make_shared<Animal>(std::move(name), age, std::move(species));
        return std::make_optional(m_animalService->addAnimal(std::move(animal)));
    } catch (const json::system_error& e) {
        std::cerr << e.what() << '\n';
        return std::nullopt;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return std::nullopt;
    }
}

std::optional<ServiceController::DeleteResourceId> ServiceController::tryDelete(std::string_view body) const {
    boost::cmatch matches;
    if(!regex_search(body.begin(), body.end(), matches, rest::routes::deleteAnimalByCompound)
        || matches.size() != 3) {
        return std::nullopt;
    }
    const auto& compoundId = matches[1];
    const auto& animalId = matches[2];
    return std::make_optional<DeleteResourceId>(compoundId, animalId);
}

}
