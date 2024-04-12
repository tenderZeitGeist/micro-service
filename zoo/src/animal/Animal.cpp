//
// Created by zeitgeist on 11.04.24.
//
#include "zoo/animal/Animal.hpp"

namespace {
    std::string speciesToString(zoo::Species species) {
        switch(species) {
            case zoo::Species::MAMMAL:
                return "Mammal";
            case zoo::Species::FISH:
                return "Fish";
            case zoo::Species::REPTILE:
                return "Reptile";
            case zoo::Species::BIRD:
                return "Bird";
        }
    }
}


namespace zoo {
    Animal::Animal(std::string name, Species species, std::size_t age)
    : m_name(std::move(name))
    , m_species(species)
    , m_age(age){

    }

    const std::string& zoo::Animal::getName() const {
        return m_name;
    }

    const std::string& zoo::Animal::getSpecies() const {
        const static auto speciesString = speciesToString(m_species);
        return speciesString;
    }

    std::size_t zoo::Animal::getAge() const {
        return m_age;
    }
}
