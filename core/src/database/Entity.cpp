//
// Created by zeitgeist on 12.04.24.
//

#include <core/database/Entity.hpp>

namespace core::database {

std::size_t Entity::getId() const
{
    return m_id;
}

void Entity::setId(std::size_t id)
{
    m_id = id;
}

}
