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

    [[nodiscard]] std::shared_ptr<core::database::Entity> getEntityById(std::size_t id) override;
    [[nodiscard]] std::vector<std::shared_ptr<core::database::Entity>> getAllEntities() override;
    [[nodiscard]] std::size_t addEntity(std::shared_ptr<core::database::Entity> entity) override;
    [[nodiscard]] std::vector<std::size_t> addEntities(std::vector<std::shared_ptr<core::database::Entity>> entities) override;
    [[nodiscard]] bool deleteEntity(std::shared_ptr<core::database::Entity> entity) override;

private:
    void initCompound(std::string_view fileName);
    std::unordered_map<std::size_t, std::shared_ptr<core::database::Entity>> m_entities;
};

}
