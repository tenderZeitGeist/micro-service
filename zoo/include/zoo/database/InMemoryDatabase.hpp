//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include "DatabaseInterface.hpp"

#include <unordered_map>

class InMemoryDatabase
    : public DatabaseInterface {
public:
    explicit InMemoryDatabase();

    std::shared_ptr<Entity> getEntityById(std::size_t id) override;
    std::vector<std::shared_ptr<Entity>> getAllEntities() override;
    void addEntity(std::shared_ptr<Entity> entity) override;
    void addEntities(std::vector<std::shared_ptr<Entity>> entities) override;
    bool deleteEntity(std::shared_ptr<Entity> entity) override;

private:
    void init();
    std::unordered_map<std::size_t, std::shared_ptr<Entity>> m_entities;
};
