//
// Created by abentler on 13.04.24.
//

#include <zoo/services/AnimalServiceController.hpp>

#include <zoo/animal/Animal.hpp>
#include <zoo/compound/Compound.hpp>
#include <zoo/services/AnimalRepository.hpp>
#include <zoo/services/CompoundRepository.hpp>

#include <boost/regex.hpp>
#include <boost/json.hpp>

#include <iostream>

namespace json = boost::json;

namespace {

json::object makeHypermediaLinks(const std::string& baseUri, const std::string& resourceId) {
    return {
        { "_links", json::array{
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
        }}
    };
}

json::object makeHypermediaLinks(
    const std::string& baseUri,
    const std::string& compoundId,
    const std::string& animalId,
    const std::string& animalSpecies) {
    return {
        { "self", baseUri + "/" + compoundId + "/" + animalId },
        { "species", baseUri + "/animals/" + animalSpecies},
        { "delete", baseUri + "/animals/"}
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
    const std::string& baseUri,
    std::reference_wrapper<const zoo::Compound> comoundRef,
    const std::vector<std::reference_wrapper<const zoo::Animal>>& animalRefs) {
    const auto& compound = comoundRef.get();
    auto links = makeHypermediaLinks(baseUri, compound.getName());
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

static const std::string kCompoundsRootUri = "/compound";
static const std::string kAnimalsRootUri = "/animals";

AnimalServiceController::AnimalServiceController(std::unique_ptr<AnimalRepository> animalService, std::unique_ptr<CompoundRepository> compoundService)
    : m_animalService(std::move(animalService)),
      m_compoundService(std::move(compoundService)) {}

rest::Response AnimalServiceController::getAllCompounds() const {
    const auto compounds = m_compoundService->getAllTargetEntities();
    auto body = parse(kCompoundsRootUri, compounds);
    if (body.empty()) {
        return kNotFoundResponse;
    }
    return rest::Response{
        rest::http::status::ok,
        std::move(body),
        rest::kJson.data()
    };
}

rest::Response AnimalServiceController::getCompoundByName(const rest::Request& r) const {
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
    std::string body = parse(kCompoundsRootUri, {*compound});
    return {
        rest::http::status::ok,
        std::move(body),
        rest::kJson.data()
    };
}

rest::Response AnimalServiceController::getAnimalByName(const rest::Request& r) const {
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

rest::Response AnimalServiceController::addAnimalToCompound(const rest::Request& r) {
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

rest::Response AnimalServiceController::deleteAnimalFromCompound(const rest::Request& r) {
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

rest::Response AnimalServiceController::getAllAnimalsBySpecies(const rest::Request& r) {
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

std::string AnimalServiceController::parse(const std::string& baseUri, const std::vector<std::reference_wrapper<const Compound>>& compounds) const {
    json::object object{{"compounds", json::array()}};
    for (auto compoundRef: compounds) {
        const auto animalRefs = m_animalService->getAnimalsByIds(compoundRef.get().getAnimals());
        object["compounds"].as_array().emplace_back(toJson(baseUri, compoundRef, animalRefs));
    }
    return json::serialize(json::array().emplace_back(std::move(object)));
}

std::optional<std::size_t> AnimalServiceController::tryAdd(std::string_view body) const {
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

std::optional<AnimalServiceController::DeleteResourceId> AnimalServiceController::tryDelete(std::string_view body) const {
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
