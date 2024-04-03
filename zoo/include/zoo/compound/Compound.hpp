//
// Created by zeitgeist on 12.04.24.
//
#pragma once

#include <string>
#include <unordered_set>
#include <vector>

namespace zoo {

class Animal;

class Compound {
public:
    Compound(std::string name, std::size_t sizeInSqm);
    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] std::size_t getSize() const;
    [[nodiscard]] std::vector<std::size_t> getAnimals() const;
    [[nodiscard]] bool addAnimal(std::size_t id);
    [[nodiscard]] bool deleteAnimal(std::size_t id);

private:
    std::string m_name;
    std::size_t m_sizeInSqm;
    std::unordered_set<std::size_t> m_animals;
};

}