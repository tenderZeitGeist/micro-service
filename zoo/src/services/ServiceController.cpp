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

namespace {

std::string toJson(const zoo::Animal& animal) {
    std::stringstream stream;
    stream << "{";
    stream << "\"id\": \"" << animal.getId() << "\", ";
    stream << "\"name\": \"" << animal.getName() << "\", ";
    stream << "\"age\": " << animal.getAge() << ", ";
    stream << "\"species\": " << animal.getSpeciesString();
    stream << "}";
    return stream.str();
}

std::string toJson(const std::vector<std::reference_wrapper<const zoo::Animal>>& animals) {
    std::stringstream stream;
    for(std::size_t i = 0; i < animals.size(); ++i) {
        stream << toJson(animals[i].get());
        if(i < animals.size() - 1) {
            stream << ", ";
        }
    }
    return stream.str();
}

std::string toJson(const zoo::Compound& compound, const std::vector<std::reference_wrapper<const zoo::Animal>>& animals) {
    std::stringstream stream;
    stream << "{";
    stream << "\"id\": \"" << compound.getId() << "\", ";
    stream << "\"name\": \"" << compound.getName() << "\", ";
    stream << "\"size\": " << compound.getSize() << ", ";
    stream << "\"animals\": [";
    for (size_t i = 0; i < animals.size(); ++i) {
        stream << toJson(animals[i]);
        if (i < animals.size() - 1) {
            stream << ", ";
        }
    }
    stream << "]";
    stream << "}";
    return stream.str();
}

std::string parseResourceId(std::string_view target) {
    std::string t = target.data();
    const auto lastSlash = target.find_last_of('/');
    std::string resourceId = t.substr(lastSlash + 1);
    if (const auto paramPos = resourceId.find_first_of("?#")) {
        return resourceId.substr(0, paramPos);
    }
    return resourceId;
}

}

namespace zoo {

namespace json = boost::json;

const rest::Response kNotFoundResponse = rest::Response(
    rest::http::status::not_found,
    "The resource couldn't be located",
    rest::kTextPlain.data()
);

const rest::Response kBadRequest = rest::Response(
    rest::http::status::bad_request,
    "The request contains insufficient or false data",
    rest::kTextPlain.data()
);

const rest::Response kInternalError= rest::Response(
    rest::http::status::internal_server_error,
    "The request couldn't be processed because of an internal error",
    rest::kTextPlain.data()
);

ServiceController::ServiceController(std::unique_ptr<AnimalService> animalService, std::unique_ptr<CompoundService> compoundService)
    : m_animalService(std::move(animalService)),
      m_compoundService(std::move(compoundService)) {}

rest::Response ServiceController::getAllCompounds() const {
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

rest::Response ServiceController::getCompoundByName(const rest::Request& r) const {
    const auto compound = m_compoundService->getEntityByName(parseResourceId(r.target()));
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

rest::Response ServiceController::getAnimalByName(const rest::Request& r) const {
    const auto animal = m_animalService->getEntityByName(parseResourceId(r.target()));
    if (!animal) {
        return kNotFoundResponse;
    }
    std::string body = toJson(*animal);
    return {
        rest::http::status::ok,
        std::move(body),
        rest::kJson.data()
    };
}

rest::Response ServiceController::addAnimalToCompound(const rest::Request& r) {
    const auto compoundResourceId = parseResourceId(r.target());
    const auto compound = m_compoundService->getEntityByName(compoundResourceId);
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

    std::string body = toJson(*animal);
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
        rest::http::status::no_content
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
        std::string body = toJson({filteredView.begin(), filteredView.end()});
        return {
            rest::http::status::ok,
            std::move(body),
            rest::kJson.data()
        };
    } catch (const std::exception&) {
        return kBadRequest;
    }
}

std::string ServiceController::parse(const std::vector<std::reference_wrapper<const Compound>>& compounds) const {
    std::stringstream stream;
    for (auto compoundRef: compounds) {
        const auto& compound = compoundRef.get();
        const auto animals = m_animalService->getAnimalsByIds(compound.getAnimals());
        stream << toJson(compound, animals);
    }
    return stream.str();
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
