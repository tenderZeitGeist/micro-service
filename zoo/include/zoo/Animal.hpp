//
// Created by zeitgeist on 11.04.24.
//

#include <string>

namespace zoo {
    enum class Species {
        MAMMAL,
        FISH,
        REPTILE,
        BIRD
    };

    class Animal {
    public:
        virtual ~Animal() = default;

        [[nodiscard]] const std::string& getName() const;
        [[nodiscard]] const std::string& getSpecies() const;
        [[nodiscard]] std::size_t getAge() const;

    protected:
        Animal(std::string name, Species species, std::size_t age);

        std::string m_name;
        Species m_species;
        std::size_t m_age;
    };

}
