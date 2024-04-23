cmake_minimum_required(VERSION 3.5)

project(core CXX)

set(DIR_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(DIR_SOURCE ${CMAKE_CURRENT_LIST_DIR}/src)

set(CORE_HEADER_FILES
    "${DIR_INCLUDE}/core/database/DatabaseInterface.hpp"
    "${DIR_INCLUDE}/core/database/Entity.hpp"
    "${DIR_INCLUDE}/core/repository/RepositoryInterface.hpp"
    "${DIR_INCLUDE}/core/service/ServiceControllerInterface.hpp"
)

set(CORE_SOURCE_FILES
    "${DIR_SOURCE}/database/DatabaseInterface.cpp"
    "${DIR_SOURCE}/database/Entity.cpp"
)

add_library(
    ${PROJECT_NAME}
    STATIC
        ${CORE_HEADER_FILES}
        ${CORE_SOURCE_FILES}
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
        boost
)
