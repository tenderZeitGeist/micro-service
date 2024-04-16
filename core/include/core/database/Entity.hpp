//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include <cstddef>

#include "zoo/database/InMemoryDatabase.hpp"

namespace core::database {

class Entity {
public:
    virtual ~Entity() = default;
    [[nodiscard]] virtual std::size_t getId() const;
protected:
    std::size_t m_id{};
private:
    friend class zoo::database::InMemoryDatabase;
    void setId(std::size_t id);
};

}
