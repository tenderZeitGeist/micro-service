//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include <atomic>

class Entity {
public:
    virtual ~Entity() = default;
    [[nodiscard]] virtual std::size_t getId() const;
protected:
    std::size_t m_id{};
private:
    friend class InMemoryDatabase;
    void setId(std::size_t id);
};