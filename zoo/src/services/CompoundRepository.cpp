//
// Created by zeitgeist on 12.04.24.
//

#include <zoo/services/CompoundRepository.hpp>

#include <zoo/compound/Compound.hpp>

namespace zoo {
CompoundRepository::CompoundRepository(std::shared_ptr<core::database::DatabaseInterface> database)
    : RepositoryInterface(std::move(database)) {}

bool CompoundRepository::addAnimal(std::reference_wrapper<const Compound> compound, std::size_t animalId) const {
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

bool CompoundRepository::deleteAnimal(std::reference_wrapper<const Compound> compound, std::size_t animalId) const {
    auto entity = m_database->getEntityById(compound.get().getId());
    if(!entity) {
        return false;
    }

    auto* compoundPtr = dynamic_cast<Compound*>(entity.get());
    if(!compoundPtr) {
        return false;
    }

    return compoundPtr->deleteAnimal(animalId);
}

}
