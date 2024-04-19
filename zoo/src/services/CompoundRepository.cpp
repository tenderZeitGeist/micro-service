//
// Created by zeitgeist on 12.04.24.
//

#include <zoo/services/CompoundRepository.hpp>
#include <zoo/compound/Compound.hpp>

#include <ranges>

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

std::optional<std::string> CompoundRepository::findCompoundByAnimalId(std::size_t animalId) const {
    const auto compounds = getAllTargetEntities();
    const auto iter = std::ranges::find_if(compounds, [animalId](auto compoundRef) {
        return compoundRef.get().hasAnimal(animalId);
    });
    if(iter != compounds.end()) {
        return std::make_optional<std::string>(iter->get().getName());
    }
    return std::nullopt;
}

}
