cmake_minimum_required(VERSION 3.5)

project(zoo CXX)

set(DIR_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(DIR_SOURCE ${CMAKE_CURRENT_LIST_DIR}/src)

set(ZOO_HEADER_FILES
        "${DIR_INCLUDE}/zoo/animal/Animal.hpp"
        "${DIR_INCLUDE}/zoo/animal/Bird.hpp"
        "${DIR_INCLUDE}/zoo/animal/Fish.hpp"
        "${DIR_INCLUDE}/zoo/animal/Mammal.hpp"
        "${DIR_INCLUDE}/zoo/animal/Reptile.hpp"

        "${DIR_INCLUDE}/zoo/compound/Compound.hpp"

        "${DIR_INCLUDE}/zoo/database/DatabaseInterface.hpp"
        "${DIR_INCLUDE}/zoo/database/Entity.hpp"
        "${DIR_INCLUDE}/zoo/database/InMemoryDatabase.hpp"

        "${DIR_INCLUDE}/zoo/services/AnimalService.hpp"
        "${DIR_INCLUDE}/zoo/services/CompoundService.hpp"
        "${DIR_INCLUDE}/zoo/services/ServiceInterface.hpp"
)

set(ZOO_SOURCE_FILES
        "${DIR_SOURCE}/animal/Animal.cpp"

        "${DIR_SOURCE}/compound/Compound.cpp"

        "${DIR_SOURCE}/database/Entity.cpp"
        "${DIR_SOURCE}/database/InMemoryDatabase.cpp"

        "${DIR_SOURCE}/services/AnimalService.cpp"
        "${DIR_SOURCE}/services/CompoundService.cpp"
)

add_library(
        ${PROJECT_NAME}
        STATIC
        ${ZOO_HEADER_FILES}
        ${ZOO_SOURCE_FILES}
)

target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${DIR_INCLUDE}
        PRIVATE
        ${DIR_SOURCE}
)

set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES
        LINKER_LANGUAGE CXX
)
