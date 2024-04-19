//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include <core/repository/RepositoryInterface.hpp>

namespace zoo {

class Compound;

    class CompoundRepository
        : public core::repository::RepositoryInterface<Compound>{
    public:
        explicit CompoundRepository(std::shared_ptr<core::database::DatabaseInterface> database);
        [[nodiscard]] bool addAnimal(std::reference_wrapper<const Compound> compound, std::size_t animalId) const;
        [[nodiscard]] bool deleteAnimal(std::reference_wrapper<const Compound> compound, std::size_t animalId) const;
        [[nodiscard]] std::optional<std::string> findCompoundByAnimalId(std::size_t animalId) const;
    };

}
