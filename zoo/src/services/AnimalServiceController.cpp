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

#include <iostream>

namespace json = boost::json;

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
    std::string body = parse({*compound});
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
    std::string body = json::serialize(json::array().emplace_back(hateos::animals::toJson("", *animal)));
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

    const auto matchedCompound = m_compoundService->getEntityByName(matches[1].str());
    if (!matchedCompound) {
        return kNotFoundResponse;
    }
    const auto& compound = matchedCompound.value().get();

    const auto createdAnimal = tryAdd(r.body());
    if(!createdAnimal) {
        return kBadRequest;
    }

    if(!m_compoundService->addAnimal(*matchedCompound, *createdAnimal)) {
        [[maybe_unused]] const auto _ = m_animalService->deleteAnimal(*createdAnimal);
        return kBadRequest;
    }

    const auto matchedAnimal = m_animalService->getEntityById(*createdAnimal);
    if(!matchedAnimal) {
        return kInternalError;
    }

    std::string body = json::serialize(json::array().emplace_back(hateos::animals::toJson(compound.getName(), *matchedAnimal)));
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
        std::string body = json::serialize(json::array().emplace_back(hateos::animals::toJson("",{filteredView.begin(), filteredView.end()})));
        return {
            rest::http::status::ok,
            std::move(body),
            rest::kJson.data()
        };
    } catch (const std::exception&) {
        return kBadRequest;
    }
}

std::string AnimalServiceController::parse(const std::vector<std::reference_wrapper<const Compound>>& compounds) const {
    json::object object{{"compounds", json::array()}};
    for (auto compoundRef: compounds) {
        const auto animalRefs = m_animalService->getAnimalsByIds(compoundRef.get().getAnimals());
        object["compounds"].as_array().emplace_back(hateos::compounds::toJson(compoundRef, animalRefs));
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
