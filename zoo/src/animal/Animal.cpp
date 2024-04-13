//
// Created by zeitgeist on 11.04.24.
//
#include "zoo/animal/Animal.hpp"

#include <cassert>

namespace {

std::string speciesToString(zoo::Species species) {
    switch (species) {
        case zoo::Species::MAMMAL:
            return "Mammal";
        case zoo::Species::FISH:
            return "Fish";
        case zoo::Species::REPTILE:
            return "Reptile";
        case zoo::Species::BIRD:
            return "Bird";
    }
    assert(false && "Invalid species param was passed.");
    return "";
}

}

namespace zoo {

Animal::Animal(std::string name, Species species, std::size_t age)
    : m_name(std::move(name))
    , m_species(species)
    , m_age(age) {}

const std::string& Animal::getName() const {
    return m_name;
}

const std::string& Animal::getSpecies() const {
    if(m_speciesString.empty()) {
        m_speciesString = speciesToString(m_species);
    }
    return m_speciesString;
}

std::size_t Animal::getAge() const {
    return m_age;
}

bool operator==(const Animal& rhs, const Animal& lhs) {
    return rhs.getAge() == lhs.getAge()
           && rhs.getName() == lhs.getName()
           && rhs.getSpecies() == lhs.getSpecies();
}

}
