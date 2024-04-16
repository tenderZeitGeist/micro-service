//
// Created by zeitgeist on 11.04.24.
//

#pragma once

#include <core/database/Entity.hpp>

#include <array>
#include <string>

namespace zoo {
    enum class Species {
        MAMMAL,
        FISH,
        REPTILE,
        BIRD,
    };

    class Animal
        : public core::database::Entity{
        static const std::array<const std::string, 4> kSpeciesStrings;
    public:
        explicit Animal(std::string name, std::size_t age, Species species);
        explicit Animal(std::string name, std::size_t age, std::string species);
        [[nodiscard]] const std::string& getName() const;
        [[nodiscard]] Species getSpecies() const;
        [[nodiscard]] const std::string& getSpeciesString() const;
        [[nodiscard]] std::size_t getAge() const;
        friend bool operator==(const Animal& rhs, const Animal& lhs);

        static std::string speciesToString(Species species);
        static Species stringToSpecies(const std::string& speciesString);

    private:
        std::string m_name;
        Species m_species;
        std::string m_speciesString;
        std::size_t m_age;
    };

}
