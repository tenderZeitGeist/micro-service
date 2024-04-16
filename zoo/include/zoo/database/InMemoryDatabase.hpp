//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include <core/database/DatabaseInterface.hpp>

#include <unordered_map>

namespace zoo::database {

class InMemoryDatabase
    : public core::database::DatabaseInterface {
public:
    explicit InMemoryDatabase();

    std::shared_ptr<core::database::Entity> getEntityById(std::size_t id) override;
    std::vector<std::shared_ptr<core::database::Entity>> getAllEntities() override;
    std::size_t addEntity(std::shared_ptr<core::database::Entity> entity) override;
    std::vector<std::size_t> addEntities(std::vector<std::shared_ptr<core::database::Entity>> entities) override;
    bool deleteEntity(std::shared_ptr<core::database::Entity> entity) override;

private:
    void initCompound(std::string_view fileName);
    std::unordered_map<std::size_t, std::shared_ptr<core::database::Entity>> m_entities;
};

}
