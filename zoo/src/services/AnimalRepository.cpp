//
// Created by zeitgeist on 12.04.24.
//

#include <zoo/services/AnimalRepository.hpp>
#include <zoo/animal/Animal.hpp>

#include <algorithm>
#include <ranges>

namespace zoo {

AnimalRepository::AnimalRepository(std::shared_ptr<core::database::DatabaseInterface> database)
    : RepositoryInterface(std::move(database)){}

std::vector<Animal> AnimalRepository::getAnimalsByIds(const std::vector<std::size_t>& ids) const {
    auto animals = getAllTargetEntities();
    const auto tail = std::ranges::partition(
        animals,
        [&ids](const Animal& animal) { return std::ranges::find(ids, animal.getId()) != ids.end(); }
    );
    return {std::make_move_iterator(animals.begin()), std::make_move_iterator(tail.begin())};
}

std::size_t AnimalRepository::addAnimal(std::shared_ptr<Animal> animal) const {
    return m_database->addEntity(std::move(animal));
}

bool AnimalRepository::deleteAnimal(std::size_t id) const {
    if(auto animal = m_database->getEntityById(id)) {
        return m_database->deleteEntity(std::move(animal));
    }
    return false;
}

}
