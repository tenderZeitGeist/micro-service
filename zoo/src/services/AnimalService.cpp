//
// Created by zeitgeist on 12.04.24.
//

#include "zoo/services/AnimalService.hpp"

#include <algorithm>
#include <ranges>

namespace zoo {

AnimalService::AnimalService(std::shared_ptr<DatabaseInterface> database)
    : ServiceInterface(std::move(database)){}

std::vector<std::reference_wrapper<const Animal>> AnimalService::getAnimalsByIds(const std::vector<std::size_t>& ids) const {
    const auto animals = getAllTargetEntities();
    auto filteredView = animals | std::views::filter([&ids](auto animal) {
        return std::ranges::any_of(ids, [animal](std::size_t id){ return animal.get().getId() == id; });
    }) | std::views::common;
    return {filteredView.begin(), filteredView.end()};
}

}
