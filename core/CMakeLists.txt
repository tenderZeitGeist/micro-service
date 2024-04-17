cmake_minimum_required(VERSION 3.5)

project(core CXX)

set(DIR_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(DIR_SOURCE ${CMAKE_CURRENT_LIST_DIR}/src)

set(SERVICES_HEADER_FILES
    "${DIR_INCLUDE}/core/database/DatabaseInterface.hpp"
    "${DIR_INCLUDE}/core/database/Entity.hpp"

    "${DIR_INCLUDE}/core/repository/ServiceControllerInterface.hpp"
    "${DIR_INCLUDE}/core/repository/RepositoryInterface.hpp"
)

set(SERVICES_SOURCE_FILES
    "${DIR_SOURCE}/database/DatabaseInterface.cpp"
    "${DIR_SOURCE}/database/Entity.cpp"
)

add_library(
    ${PROJECT_NAME}
    STATIC
    ${SERVICES_HEADER_FILES}
    ${SERVICES_SOURCE_FILES}
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

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    zoo
    ${Boost_LIBRARIES}
)
