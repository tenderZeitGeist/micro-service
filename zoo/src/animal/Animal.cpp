//
// Created by zeitgeist on 11.04.24.
//
#include "zoo/animal/Animal.hpp"

#include <cassert>
#include <sstream>

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

zoo::Species stringToSpecies(const std::string& speciesString) {
    if (speciesString == "Mammal") {
        return zoo::Species::MAMMAL;
    }
    if (speciesString == "Fish") {
        return zoo::Species::FISH;
    }
    if (speciesString == "Reptile") {
        return zoo::Species::REPTILE;
    }
    if (speciesString == "Bird") {
        return zoo::Species::BIRD;
    }
    assert(false && "Invalid species string was passed.");
    return zoo::Species::INVALID; // Or some other default value
}

}

namespace zoo {

Animal::Animal(std::string name, std::size_t age, Species species)
    : m_name(std::move(name))
    ,  m_species(species)
    ,  m_age(age) {}

Animal::Animal(std::string name, std::size_t age, std::string species)
    : m_name(std::move(name))
    , m_species(stringToSpecies(species))
    , m_speciesString(std::move(species))
    , m_age(age) {}

const std::string& Animal::getName() const {
    return m_name;
}

const std::string& Animal::getSpecies() const {
    if (m_speciesString.empty()) {
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
