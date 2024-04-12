//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include "ServiceInterface.hpp"

#include "zoo/animal/Animal.hpp"

namespace zoo {

    class AnimalService
        : public ServiceInterface<Animal> {
    public:
        [[nodiscard]] std::optional<std::reference_wrapper<T>> getEntityByName(const std::string& name) const override;
    };

}
