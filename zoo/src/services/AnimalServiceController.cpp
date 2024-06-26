//
// Created by abentler on 13.04.24.
//

#include <zoo/services/AnimalServiceController.hpp>

#include <zoo/animal/Animal.hpp>
#include <zoo/compound/Compound.hpp>
#include <zoo/services/AnimalRepository.hpp>
#include <zoo/services/CompoundRepository.hpp>
#include <zoo/Utils.hpp>

#include <boost/regex.hpp>
#include <boost/json.hpp>

namespace json = boost::json;

namespace {

std::optional<std::string> extractResourceId(std::string_view target, const rest::Endpoint& endpoint) {
    boost::cmatch matches;
    if (boost::regex_search(target.begin(), target.end(), matches, endpoint) && matches.size() == 2) {
        return std::make_optional<std::string>(matches[1].str());
    }
    return std::nullopt;
}

std::optional<zoo::Species> speciesStringToSpecies(const std::string& speciesString)
try {
    return std::make_optional<zoo::Species>(zoo::Animal::stringToSpecies(speciesString));
} catch (const std::invalid_argument&) {
    return std::nullopt;
}
}

namespace zoo {

static const rest::Response kNotFoundResponse {
    rest::http::status::not_found,
    "The resource couldn't be located",
    rest::kTextPlain
};

static const rest::Response kBadRequest {
    rest::http::status::bad_request,
    "The request contains insufficient or false data",
    rest::kTextPlain
};

static const rest::Response kInternalError {
    rest::http::status::internal_server_error,
    "The request couldn't be processed because of an internal error",
    rest::kTextPlain
};

AnimalServiceController::AnimalServiceController(std::unique_ptr<AnimalRepository> animalService, std::unique_ptr<CompoundRepository> compoundService)
    : m_animalService(std::move(animalService)),
      m_compoundService(std::move(compoundService)) {}

rest::Response AnimalServiceController::getAllCompounds() const {
    const auto compounds = m_compoundService->getAllTargetEntities();
    auto body = parse(compounds);
    if (body.empty()) {
        return kNotFoundResponse;
    }
    return rest::Response{
        rest::http::status::ok,
        std::move(body),
        rest::kJson
    };
}

rest::Response AnimalServiceController::getCompoundByName(const rest::Request& r) const {
    const auto resourceId = extractResourceId(r.target(), rest::routes::getCompoundByName);
    if (!resourceId) {
        return kBadRequest;
    }

    const auto compound = m_compoundService->getEntityByName(*resourceId);
    if (!compound) {
        return kNotFoundResponse;
    }

    std::string body = parse({*compound});
    return {
        rest::http::status::ok,
        std::move(body),
        rest::kJson
    };
}

rest::Response AnimalServiceController::getAnimalByName(const rest::Request& r) const {
    const auto resourceId = extractResourceId(r.target(), rest::routes::getAnimalByName);
    if (!resourceId) {
        return kBadRequest;
    }

    const auto matchedAnimal = m_animalService->getEntityByName(*resourceId);
    if (!matchedAnimal) {
        return kNotFoundResponse;
    }

    const auto animal = matchedAnimal.value().get();
    const auto matchedCompound = m_compoundService->findCompoundByAnimalId(animal.getId());
    if (!matchedCompound) {
        return kInternalError;
    }

    std::string body = json::serialize(json::array().emplace_back(hateos::animals::toJson(*matchedCompound, animal)));
    return {
        rest::http::status::ok,
        std::move(body),
        rest::kJson
    };
}

rest::Response AnimalServiceController::addAnimalToCompound(const rest::Request& r) {
    const auto resourceId = extractResourceId(r.target(), rest::routes::postAnimalByCompound);
    if (!resourceId) {
        return kBadRequest;
    }

    const auto matchedCompound = m_compoundService->getEntityByName(*resourceId);
    if (!matchedCompound) {
        return kNotFoundResponse;
    }

    const auto& compound = matchedCompound.value().get();
    const auto createdAnimal = tryAdd(r.body());
    if (!createdAnimal) {
        return kBadRequest;
    }

    const auto animalId = *createdAnimal;
    if (!m_compoundService->addAnimal(std::cref(compound), animalId)) {
        [[maybe_unused]] const auto _ = m_animalService->deleteAnimal(animalId);
        return kBadRequest;
    }

    const auto matchedAnimal = m_animalService->getEntityById(animalId);
    if (!matchedAnimal) {
        return kInternalError;
    }

    std::string body = json::serialize(json::array().emplace_back(hateos::animals::toJson(compound.getName(), *matchedAnimal)));
    return {
        rest::http::status::ok,
        std::move(body),
        rest::kJson
    };
}

rest::Response AnimalServiceController::deleteAnimalFromCompound(const rest::Request& r) {
    const auto deleteResourceId = tryDelete(r.target());
    if (!deleteResourceId) {
        return kBadRequest;
    }

    const auto& [compoundName, animalName] = *deleteResourceId;
    const auto matchedCompound = m_compoundService->getEntityByName(compoundName);
    if (!matchedCompound) {
        return kBadRequest;
    }

    const auto matchedAnimal = m_animalService->getEntityByName(animalName);
    if (!matchedAnimal) {
        return kBadRequest;
    }

    const auto animalIds = matchedCompound.value().get().getAnimals();
    const auto searchedId = matchedAnimal.value().get().getId();

    if (!std::ranges::any_of(
        animalIds,
        [searchedId](std::size_t id) {
            return id == searchedId;
        }
    )) {
        return kBadRequest;
    }

    const auto success = m_compoundService->deleteAnimal(*matchedCompound, searchedId)
                         && m_animalService->deleteAnimal(searchedId);
    if (!success) {
        return kInternalError;
    }

    return {
        rest::http::status::no_content,
        ""
    };
}

rest::Response AnimalServiceController::getAllAnimalsBySpecies(const rest::Request& r) {
    const auto resourceId = extractResourceId(r.target(), rest::routes::getAnimalsBySpecies);
    if (!resourceId) {
        return kBadRequest;
    }

    const auto species = speciesStringToSpecies(*resourceId);
    if (!species) {
        return kBadRequest;
    }

    const auto animals = m_animalService->getAllTargetEntities();
    auto filteredView = animals
                        | std::ranges::views::filter(
                            [species](auto animal) {
                                return animal.get().getSpecies() == species;
                            }
                        )
                        | std::views::common;

    std::string body;
    try {
        body = parse(std::vector<std::reference_wrapper<const Animal>>{filteredView.begin(), filteredView.end()});
    } catch (const std::logic_error&) {
        return kInternalError;
    }
    return {
        rest::http::status::ok,
        std::move(body),
        rest::kJson
    };
}

std::string AnimalServiceController::parse(const std::vector<std::reference_wrapper<const Compound>>& compounds) const {
    json::object object{{"compounds", json::array()}};
    for (auto compoundRef: compounds) {
        const auto animalRefs = m_animalService->getAnimalsByIds(compoundRef.get().getAnimals());
        object["compounds"].as_array().emplace_back(hateos::compounds::toJson(compoundRef, animalRefs));
    }
    return json::serialize(json::array().emplace_back(std::move(object)));
}

std::string AnimalServiceController::parse(const std::vector<std::reference_wrapper<const Animal>>& animals) const {
    json::object object{{"animals", json::array()}};
    for (auto animalRef: animals) {
        const auto compoundId = m_compoundService->findCompoundByAnimalId(animalRef.get().getId());
        if (!compoundId) {
            throw std::logic_error("An animal needs to be linked to a compound. Aborting process.");
        }
        object["animals"].as_array().emplace_back(hateos::animals::toJson(*compoundId, animalRef));
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
    } catch (const json::system_error&) {
        return std::nullopt;
    } catch (const std::invalid_argument&) {
        return std::nullopt;
    }
}

std::optional<AnimalServiceController::DeleteResourceId> AnimalServiceController::tryDelete(std::string_view body) const {
    boost::cmatch matches;
    if (!regex_search(body.begin(), body.end(), matches, rest::routes::deleteAnimalByCompound)
        || matches.size() != 3) {
        return std::nullopt;
    }
    const auto& compoundId = matches[1];
    const auto& animalId = matches[2];
    return std::make_optional<DeleteResourceId>(compoundId, animalId);
}

}
