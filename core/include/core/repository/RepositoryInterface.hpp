//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include <core/database/DatabaseInterface.hpp>

#include <memory>
#include <string>
#include <optional>
#include <ranges>
#include <vector>

namespace core::repository {
template<typename T>
class RepositoryInterface {
public:
    virtual ~RepositoryInterface() = default;

    [[nodiscard]] std::vector<std::reference_wrapper<const T>> getAllTargetEntities() const {
        const auto entities = m_database->getAllEntities();
        auto filteredView = entities
        | std::ranges::views::filter([](const auto& entity) { return dynamic_cast<const T*>(entity.get()) != nullptr; })
        | std::views::transform([](const auto& entity){ return std::cref(*static_cast<const T*>(entity.get())); })
        | std::views::common;

        return {filteredView.begin(), filteredView.end()};
    }

    [[nodiscard]] std::optional<std::reference_wrapper<const T>> getEntityByName(const std::string& name) const {
        const auto entities = m_database->getAllEntities();
        const auto iter = std::ranges::find_if(
            entities,
            [&name](const auto& entity) {
                if (const auto* targetType = dynamic_cast<const T*>(entity.get())) {
                    return targetType->getName() == name;
                }
                return false;
            }
        );

        if (iter != entities.end()) {
            return std::make_optional(std::ref(static_cast<T&>(**iter)));
        }

        return std::nullopt;
    }

    [[nodiscard]] std::optional<std::reference_wrapper<const T>> getEntityById(std::size_t id) const {
        const auto entities = m_database->getAllEntities();
        const auto iter = std::ranges::find_if(
            entities,
            [id](const auto& entity) {
                if (const auto* targetType = dynamic_cast<const T*>(entity.get())) {
                    return targetType->getId() == id;
                }
                return false;
            }
        );

        if (iter != entities.end()) {
            return std::make_optional(std::ref(static_cast<T&>(**iter)));
        }

        return std::nullopt;
    }

protected:
    explicit RepositoryInterface(std::shared_ptr<database::DatabaseInterface> database)
        : m_database(std::move(database)) {}

    std::shared_ptr<database::DatabaseInterface> m_database;
};

}
