//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include <core/repository/RepositoryInterface.hpp>

#include <vector>

namespace zoo {

class Animal;

    class AnimalRepository
        : public core::repository::RepositoryInterface<Animal> {
    public:
        explicit AnimalRepository(std::shared_ptr<core::database::DatabaseInterface> database);
        [[nodiscard]] std::vector<std::reference_wrapper<const Animal>> getAnimalsByIds(const std::vector<std::size_t>& ids) const;
        [[nodiscard]] std::size_t addAnimal(std::shared_ptr<Animal> animal) const;
        [[nodiscard]] bool deleteAnimal(std::size_t id) const;
    };

}
