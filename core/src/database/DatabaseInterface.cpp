//
// Created by zeitgeist on 17.04.24.
//

#include "core/database/DatabaseInterface.hpp"

#include "core/database/Entity.hpp"

namespace core::database {

std::atomic_size_t DatabaseInterface::kCounter = 0;

void DatabaseInterface::setEntityId(Entity* entity) const {
    entity->setId(++kCounter);
}

}
