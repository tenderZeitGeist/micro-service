//
// Created by zeitgeist on 12.04.24.
//

#pragma once

#include <memory>
#include <string>
#include <optional>
#include <vector>

#include <zoo/database/DatabaseInterface.hpp>

namespace zoo {
template<typename T>
class ServiceInterface {
public:
    virtual ~ServiceInterface() = default;
    [[nodiscard]] std::optional<std::reference_wrapper<const T>> getEntityByName(const std::string& name) const;

protected:
    explicit ServiceInterface(std::shared_ptr<DatabaseInterface> database);
    std::shared_ptr<DatabaseInterface> m_database;
};

template<typename T>
ServiceInterface<T>::ServiceInterface(std::shared_ptr<DatabaseInterface> database)
    : m_database(std::move(database)) {}

template<typename T>
[[nodiscard]] std::optional<std::reference_wrapper<const T>> ServiceInterface<T>::getEntityByName(const std::string& name) const {
    const auto entities = m_database->getAllEntities();
    const auto iter = std::ranges::find_if(entities, [&name](const auto& entity) {
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
}
