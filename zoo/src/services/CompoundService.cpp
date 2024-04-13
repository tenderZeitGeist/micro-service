//
// Created by zeitgeist on 12.04.24.
//

#include "zoo/services/CompoundService.hpp"

namespace zoo {

CompoundService::CompoundService(std::shared_ptr<DatabaseInterface> database)
    : ServiceInterface(std::move(database)) {}


}
