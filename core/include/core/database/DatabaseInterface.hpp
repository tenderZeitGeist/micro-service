//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include <atomic>
#include <memory>
#include <vector>

namespace core::database {

class Entity;

class DatabaseInterface {
    static std::atomic_size_t kCounter;
public:
    virtual ~DatabaseInterface() = default;
    [[nodiscard]] virtual std::shared_ptr<Entity> getEntityById(std::size_t id) = 0;
    [[nodiscard]] virtual std::vector<std::shared_ptr<Entity>> getAllEntities() = 0;
    [[nodiscard]] virtual bool deleteEntity(std::shared_ptr<Entity> entity) = 0;
    virtual std::size_t addEntity(std::shared_ptr<Entity> entity) = 0;
    virtual std::vector<std::size_t> addEntities(std::vector<std::shared_ptr<Entity>> entities) = 0;
protected:
    void setEntityId(Entity* entity) const;
};

}