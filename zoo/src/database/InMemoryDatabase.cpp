
// Created by zeitgeist on 12.04.24.
//

#include "zoo/database/InMemoryDatabase.hpp"

#include "zoo/animal/Animal.hpp"
#include "zoo/database/Entity.hpp"
#include "zoo/compound/Compound.hpp"

#include <atomic>
#include <ranges>
#include <fstream>
#include <filesystem>

namespace {
std::atomic_size_t kCounter = 0;
}

InMemoryDatabase::InMemoryDatabase()
{
    initPettingZoo();
}

std::shared_ptr<Entity> InMemoryDatabase::getEntityById(std::size_t id) {
    if(const auto iter = m_entities.find(id); iter != m_entities.end())
    {
        return iter->second;
    }
    return {};
}

std::vector<std::shared_ptr<Entity>> InMemoryDatabase::getAllEntities() {
    std::vector<std::shared_ptr<Entity>> results;
    results.reserve(m_entities.size());
    std::ranges::copy(m_entities | std::views::values, std::back_inserter(results));
    return results;
}

void InMemoryDatabase::addEntity(std::shared_ptr<Entity> entity) {
    entity->setId(++kCounter);
    m_entities.emplace(entity->getId(), std::move(entity));
}

bool InMemoryDatabase::deleteEntity(std::shared_ptr<Entity> entity) {
    const auto id = entity->getId();
    if(m_entities.contains(id))
    {
         m_entities.erase(id);
         return true;
    }
    return false;
}

void InMemoryDatabase::addEntities(std::vector<std::shared_ptr<Entity>> entities){
    for(auto& entity : entities) {
        addEntity(std::move(entity));
    }
}

void InMemoryDatabase::initPettingZoo() {
    std::ifstream file("PettingZoo.txt");
    if(!file.is_open()) {
        std::abort();
    }
    std::string compoundName;
    std::size_t sizeInSqm;
    file >> compoundName >> sizeInSqm;

    std::vector<std::shared_ptr<Entity>> animals;
    std::string line;
    while(std::getline(file, line)) {
        std::istringstream is(line);
        std::string name;
        std::size_t age;
        std::string species;
        if(is >> name >> age >> species) {
            animals.emplace_back(std::make_shared<zoo::Animal>(std::move(name), age, std::move(species)));
        }
    }
    // const auto animalRefs = addEntities(std::move(animals));
    // Add transform for animal id;

    std::shared_ptr<zoo::Compound> pettingZoo = std::make_shared<zoo::Compound>(std::move(compoundName), sizeInSqm);
}
