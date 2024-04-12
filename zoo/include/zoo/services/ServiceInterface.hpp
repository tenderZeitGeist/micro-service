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
        [[nodiscard]] virtual std::optional<std::reference_wrapper<T>> getEntityByName(const std::string& name) const = 0;
    protected:
        explicit ServiceInterface(std::shared_ptr<DatabaseInterface> database);
        std::shared_ptr<DatabaseInterface> m_database;
    };

    template<typename T>
    ServiceInterface<T>::ServiceInterface(std::shared_ptr<DatabaseInterface> database)
        : m_database(std::move(database))
    {
    }
}
