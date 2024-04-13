//
// Created by abentler on 13.04.24.
//

#include "zoo/services/ServiceController.hpp"

#include "zoo/services/AnimalService.hpp"
#include "zoo/services/CompoundService.hpp"
#include "zoo/services/ServiceInterface.hpp"

namespace {
std::string toJson(const zoo::Animal& animal) {
    std::stringstream stream;
    stream << "{";
    stream << "\"id\": \"" << animal.getId() << "\", ";
    stream << "\"name\": \"" << animal.getName() << "\", ";
    stream << "\"age\": " << animal.getAge() << ", ";
    stream << "\"species\": " << animal.getSpecies();
    stream << "}";
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
}

namespace zoo {

const rest::Response kNotFoundResponse = rest::Response(rest::http::status::not_found);

ServiceController::ServiceController(std::unique_ptr<AnimalService> animalService, std::unique_ptr<CompoundService> compoundService)
    : m_animalService(std::move(animalService)),
      m_compoundService(std::move(compoundService)) {}

rest::Response ServiceController::getAllCompounds(const rest::Request& r) const {
    const auto compounds = m_compoundService->getAllTargetEntities();
    std::stringstream stream;
    return kNotFoundResponse;
}

rest::Response ServiceController::getCompoundByName(const rest::Request& r) const {
    const auto compound = m_compoundService->getEntityByName(r.body());
    if(!compound) {
        return kNotFoundResponse;
    }
}

rest::Response ServiceController::getAnimalByName(const rest::Request& r) const {
    return kNotFoundResponse;
}

rest::Response ServiceController::addAnimalToCompound(const rest::Request& r) {
    return kNotFoundResponse;
}

rest::Response ServiceController::deleteAnimalFromCompound(const rest::Request& r) {
    return kNotFoundResponse;
}

rest::Response ServiceController::getAllAnimalsBySpecies(const rest::Request& r) {
    return kNotFoundResponse;
}

}
