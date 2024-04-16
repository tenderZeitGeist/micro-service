//
// Created by zeitgeist on 12.04.24.
//

#include "zoo/compound/Compound.hpp"

#include <memory>
#include <ranges>

namespace zoo {

Compound::Compound(std::string name, std::size_t sizeInSqm)
    : m_name(std::move(name))
    , m_sizeInSqm(sizeInSqm) {}

const std::string& Compound::getName() const {
    return m_name;
}

std::size_t Compound::getSize() const {
    return m_sizeInSqm;
}

std::vector<std::size_t> Compound::getAnimals() const {
    std::vector<std::size_t> results;
    results.reserve(m_animals.size());
    std::ranges::copy(m_animals, std::back_inserter(results));
    return results;
}

bool Compound::addAnimal(std::size_t id) {
    if(m_animals.contains(id)) {
       return false;
    }
    m_animals.emplace(id);
    return true;
}

bool Compound::deleteAnimal(std::size_t id) {
    if(!m_animals.contains(id)) {
        return false;
    }
    m_animals.erase(id);
    return true;
}

}
