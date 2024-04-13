//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include "ServiceInterface.hpp"

#include "zoo/compound/Compound.hpp"

namespace zoo {

    class CompoundService
        : public ServiceInterface<Compound>{
    public:
        explicit CompoundService(std::shared_ptr<DatabaseInterface> database);
    };

}