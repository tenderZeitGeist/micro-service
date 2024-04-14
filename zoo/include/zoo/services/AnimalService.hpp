//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include "ServiceInterface.hpp"
#include "zoo/animal/Animal.hpp"

#include <vector>

namespace zoo {

    class AnimalService
        : public ServiceInterface<Animal> {
    public:
        explicit AnimalService(std::shared_ptr<DatabaseInterface> database);
        [[nodiscard]] std::vector<std::reference_wrapper<const Animal>> getAnimalsByIds(const std::vector<std::size_t>& ids) const;
    };

}
