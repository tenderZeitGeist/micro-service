cmake_minimum_required(VERSION 3.5)

project(rest CXX)

set(DIR_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(DIR_SOURCE ${CMAKE_CURRENT_LIST_DIR}/src)

set(REST_HEADER_FILES
    "${DIR_INCLUDE}/rest/Alias.hpp"
    "${DIR_INCLUDE}/rest/Connection.hpp"
    "${DIR_INCLUDE}/rest/Controller.hpp"
    "${DIR_INCLUDE}/rest/HttpWorker.hpp"
    "${DIR_INCLUDE}/rest/Response.hpp"
    "${DIR_INCLUDE}/rest/Request.hpp"
    "${DIR_INCLUDE}/rest/Route.hpp"
)

set(REST_SOURCE_FILES
    "${DIR_SOURCE}/Connection.cpp"
    "${DIR_SOURCE}/Controller.cpp"
    "${DIR_SOURCE}/HttpWorker.cpp"
)

add_library(
    ${PROJECT_NAME}
    STATIC
    ${REST_HEADER_FILES}
    ${REST_SOURCE_FILES}
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
    core
    ${Boost_LIBRARIES}
)
