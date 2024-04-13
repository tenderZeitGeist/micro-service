
// Created by zeitgeist on 12.04.24.
//

#include "zoo/database/InMemoryDatabase.hpp"
#include "zoo/database/Entity.hpp"

#include <atomic>
#include <ranges>

namespace {
std::atomic_size_t kCounter = 0;
}

InMemoryDatabase::InMemoryDatabase()
{
    init();
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
