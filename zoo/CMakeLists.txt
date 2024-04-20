cmake_minimum_required(VERSION 3.5)

project(zoo CXX)

set(DIR_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(DIR_SOURCE ${CMAKE_CURRENT_LIST_DIR}/src)
set(DIR_PRIVATE ${CMAKE_CURRENT_LIST_DIR}/private)

set(ZOO_HEADER_FILES
    "${DIR_INCLUDE}/zoo/animal/Animal.hpp"
    "${DIR_INCLUDE}/zoo/compound/Compound.hpp"
    "${DIR_INCLUDE}/zoo/database/InMemoryDatabase.hpp"
    "${DIR_INCLUDE}/zoo/services/AnimalServiceController.hpp"
    "${DIR_INCLUDE}/zoo/services/AnimalRepository.hpp"
    "${DIR_INCLUDE}/zoo/services/CompoundRepository.hpp"
    "${DIR_PRIVATE}/zoo/Utils.hpp"
)

set(ZOO_SOURCE_FILES
    "${DIR_SOURCE}/animal/Animal.cpp"
    "${DIR_SOURCE}/compound/Compound.cpp"
    "${DIR_SOURCE}/database/InMemoryDatabase.cpp"
    "${DIR_SOURCE}/services/AnimalServiceController.cpp"
    "${DIR_SOURCE}/services/AnimalRepository.cpp"
    "${DIR_SOURCE}/services/CompoundRepository.cpp"
    "${DIR_PRIVATE}/zoo/Utils.cpp"
)

file(
    COPY
        ${CMAKE_SOURCE_DIR}/extras/MarineSanctuary.txt
        ${CMAKE_SOURCE_DIR}/extras/PettingZoo.txt
        ${CMAKE_SOURCE_DIR}/extras/Safari.txt
        ${CMAKE_SOURCE_DIR}/extras/TropicalAviary.txt
        ${CMAKE_SOURCE_DIR}/extras/ReptileRetreat.txt
    DESTINATION
        ${CMAKE_BINARY_DIR}
)

add_library(
    ${PROJECT_NAME}
    STATIC
        ${ZOO_HEADER_FILES}
        ${ZOO_SOURCE_FILES}
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        core
        rest
        ${Boost_LIBRARIES}
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        ${DIR_INCLUDE}
    PRIVATE
        ${DIR_SOURCE}
        ${DIR_PRIVATE}
        ${Boost_ROOT}
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
        LINKER_LANGUAGE CXX
)
