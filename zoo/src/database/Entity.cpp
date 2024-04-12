//
// Created by zeitgeist on 12.04.24.
//

#include "zoo/database/Entity.hpp"

std::size_t Entity::getId() const
{
    return m_id;
}

void Entity::setId(std::size_t id)
{
    m_id = id;
}
