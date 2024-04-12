//
// Created by zeitgeist on 12.04.24.
//

#include "zoo/services/AnimalService.hpp"

namespace zoo
{

std::optional<std::reference_wrapper<Animal>> AnimalService::getEntityByName(const std::string& name) const
{
    const auto entities = m_database->getAllEntities();
    return std::nullopt;
}

}
