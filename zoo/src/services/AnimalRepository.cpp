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

std::vector<std::reference_wrapper<const Animal>> AnimalRepository::getAnimalsByIds(const std::vector<std::size_t>& ids) const {
    const auto animals = getAllTargetEntities();
    auto filteredView = animals | std::views::filter([&ids](auto animal) {
        return std::ranges::any_of(ids, [animal](std::size_t id){ return animal.get().getId() == id; });
    }) | std::views::common;
    return {filteredView.begin(), filteredView.end()};
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
