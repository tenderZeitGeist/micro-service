//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include <core/database/DatabaseInterface.hpp>

#include <memory>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

namespace core::repository {
template<typename T>
class RepositoryInterface {
public:
    virtual ~RepositoryInterface() = default;

    [[nodiscard]] std::vector<T> getAllTargetEntities() const {
        const auto entities = m_database->getAllEntities();
        // clang-format off
        auto filteredView = entities
                            | std::ranges::views::filter([](const auto& entity) { return dynamic_cast<const T*>(entity.get()) != nullptr; })
                            | std::views::transform([](const auto& entity) { return T(static_cast<T&>(*entity)); })
                            | std::views::common;
        // clang-format on
        return {filteredView.begin(), filteredView.end()};
    }

    [[nodiscard]] std::optional<T> getEntityByName(const std::string& name) const {
        const auto entities = m_database->getAllEntities();
        // clang-format off
        const auto iter = std::ranges::find_if(
            entities,
            [&name](const auto& entity) {
                if (const auto* targetType = dynamic_cast<const T*>(entity.get())) {
                    return targetType->getName() == name;
                }
                return false;
            }
        );
        // clang-format on

        if (iter == entities.end()) {
            return std::nullopt;
        }

        const auto derived = dynamic_cast<const T*>(iter->get());

        if (!derived) {
            return std::nullopt;
        }

        return std::make_optional<T>(*derived);
    }

    [[nodiscard]] std::optional<T> getEntityById(std::size_t id) const {
        const auto entity = m_database->getEntityById(id);

        if (!entity) {
            return std::nullopt;
        }

        const auto derived = dynamic_cast<const T*>(entity.get());

        if (!derived) {
            return std::nullopt;
        }

        return std::make_optional<T>(*derived);
    }

protected:
    explicit RepositoryInterface(std::shared_ptr<database::DatabaseInterface> database)
        : m_database(std::move(database)) {}

    std::shared_ptr<database::DatabaseInterface> m_database;
};

}
