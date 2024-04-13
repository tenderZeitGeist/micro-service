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
        explicit AnimalService(std::shared_ptr<DatabaseInterface> database);
    };

}
