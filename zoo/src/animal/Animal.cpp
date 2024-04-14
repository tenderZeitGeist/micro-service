//
// Created by zeitgeist on 11.04.24.
//
#include "zoo/animal/Animal.hpp"

#include <algorithm>
#include <sstream>
#include <exception>

namespace zoo {

const std::array<const std::string, 4> Animal::kSpeciesStrings = {
        "Mammal",
        "Fish",
        "Reptile",
        "Bird"
    };

Animal::Animal(std::string name, std::size_t age, Species species)
    : m_name(std::move(name))
    , m_species(species)
    , m_age(age) {}

Animal::Animal(std::string name, std::size_t age, std::string species)
    : m_name(std::move(name))
    , m_species(stringToSpecies(species))
    , m_age(age) {}

const std::string& Animal::getName() const {
    return m_name;
}

Species Animal::getSpecies() const {
    return m_species;
}

const std::string& Animal::getSpeciesString() const {
    return kSpeciesStrings[static_cast<std::size_t>(m_species)];
}

std::size_t Animal::getAge() const {
    return m_age;
}

bool operator==(const Animal& rhs, const Animal& lhs) {
    return rhs.getAge() == lhs.getAge()
           && rhs.getName() == lhs.getName()
           && rhs.getSpecies() == lhs.getSpecies();
}

std::string Animal::speciesToString(Species species) {
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
    throw std::exception();
}

Species Animal::stringToSpecies(const std::string& speciesString) {
    std::string lowerCase;
    std::ranges::transform(speciesString, std::back_inserter(lowerCase), [](auto c){ return std::tolower(c); });
    if (lowerCase == "mammal") {
        return zoo::Species::MAMMAL;
    }
    if (lowerCase == "fish") {
        return zoo::Species::FISH;
    }
    if (lowerCase == "reptile") {
        return zoo::Species::REPTILE;
    }
    if (lowerCase == "bird") {
        return zoo::Species::BIRD;
    }
    throw std::exception();
}

}
