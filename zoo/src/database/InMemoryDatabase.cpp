
// Created by zeitgeist on 12.04.24.
//

#include <zoo/database/InMemoryDatabase.hpp>

#include <zoo/animal/Animal.hpp>
#include <zoo/compound/Compound.hpp>

#include <atomic>
#include <memory>
#include <ranges>
#include <fstream>
#include <filesystem>
#include <vector>

namespace zoo::database {

using namespace core::database;

InMemoryDatabase::InMemoryDatabase() {
    for (const auto file: {"MarineSanctuary.txt", "PettingZoo.txt", "Safari.txt", "ReptileRetreat.txt", "TropicalAviary.txt"}) {
        initCompound(file);
    }
}

std::shared_ptr<Entity> InMemoryDatabase::getEntityById(std::size_t id) {
    if (const auto iter = m_entities.find(id); iter != m_entities.end()) {
        return iter->second;
    }
    return {};
}

std::vector<std::shared_ptr<Entity>> InMemoryDatabase::getAllEntities() {
    auto view = m_entities | std::views::values | std::views::common;
    return {view.begin(), view.end()};
}

bool InMemoryDatabase::deleteEntity(std::shared_ptr<Entity> entity) {
    const auto id = entity->getId();
    if (m_entities.contains(id)) {
        m_entities.erase(id);
        return true;
    }
    return false;
}

std::size_t InMemoryDatabase::addEntity(std::shared_ptr<Entity> entity) {
    setEntityId(entity.get());
    const auto id = entity->getId();
    m_entities.emplace(id, std::move(entity));
    return id;
}

std::vector<std::size_t> InMemoryDatabase::addEntities(std::vector<std::shared_ptr<Entity>> entities) {
    std::vector<std::size_t> ids;
    ids.reserve(entities.size());
    for (auto& entity: entities) {
        ids.emplace_back(addEntity(std::move(entity)));
    }
    return ids;
}

void InMemoryDatabase::initCompound(std::string_view fileName) {
    std::ifstream file(fileName.data());
    if (!file.is_open()) {
        std::abort();
    }
    std::string compoundName;
    std::size_t sizeInSqm;
    file >> compoundName >> sizeInSqm;

    std::vector<std::shared_ptr<Entity>> animals;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream is(line);
        std::string name;
        std::size_t age;
        std::string species;
        if (is >> name >> age >> species) {
            animals.emplace_back(std::make_shared<zoo::Animal>(std::move(name), age, std::move(species)));
        }
    }
    auto animalIds = addEntities(std::move(animals));
    std::shared_ptr<Entity> pettingZoo = std::make_shared<zoo::Compound>(std::move(compoundName), sizeInSqm);
    auto* compoundPtr = static_cast<zoo::Compound*>(pettingZoo.get());
    for (auto id: animalIds) {
        if (!compoundPtr->addAnimal(id)) {
            std::abort();
        }
    }
    if(!addEntity(std::move(pettingZoo))) {
        std::abort();
    }
}

}