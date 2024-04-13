//
// Created by zeitgeist on 11.04.24.
//

#pragma once

#include <string>

#include <zoo/database/Entity.hpp>

namespace zoo {
    enum class Species {
        MAMMAL,
        FISH,
        REPTILE,
        BIRD,
        INVALID
    };

    class Animal
        : public Entity{
    public:
        explicit Animal(std::string name, std::size_t age, Species species);
        explicit Animal(std::string name, std::size_t age, std::string species);
        [[nodiscard]] const std::string& getName() const;
        [[nodiscard]] const std::string& getSpecies() const;
        [[nodiscard]] std::size_t getAge() const;
        friend bool operator==(const Animal& rhs, const Animal& lhs);

    private:
        std::string m_name;
        Species m_species;
        mutable std::string m_speciesString;
        std::size_t m_age;
    };

}
