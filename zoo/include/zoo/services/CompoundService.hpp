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
        [[nodiscard]] std::optional<std::reference_wrapper<Compound>> getEntityByName(const std::string& name) const override;
    };

}