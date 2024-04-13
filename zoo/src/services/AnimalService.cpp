//
// Created by zeitgeist on 12.04.24.
//

#include "zoo/services/AnimalService.hpp"

namespace zoo {

AnimalService::AnimalService(std::shared_ptr<DatabaseInterface> database)
    : ServiceInterface(std::move(database)){}

}
