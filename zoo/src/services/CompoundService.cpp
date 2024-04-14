//
// Created by zeitgeist on 12.04.24.
//

#include "zoo/services/CompoundService.hpp"

#include "zoo/animal/Animal.hpp"

namespace zoo {

CompoundService::CompoundService(std::shared_ptr<DatabaseInterface> database)
    : ServiceInterface(std::move(database)) {}

bool CompoundService::addAnimal(std::reference_wrapper<const Compound> compound, std::size_t animalId) const {
    auto entity = m_database->getEntityById(compound.get().getId());
    if(!entity) {
       return false;
    }

    auto* compoundPtr = dynamic_cast<Compound*>(entity.get());
    if(!compoundPtr) {
        return false;
    }

    return compoundPtr->addAnimal(animalId);
}

}
