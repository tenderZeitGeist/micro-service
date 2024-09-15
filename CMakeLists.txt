cmake_minimum_required(VERSION 3.25)

project(micro-service)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -Wall -Wextra -O0 -Wconversion -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
message(STATUS "Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")

option(ENABLE_UNIT_TESTS "Enables unit tests" Off)
option(ENABLE_LOGGER "Enables logging" Off)
option(ENABLE_PERFORMANCE_TIMING "Enables logging" Off)

include(Boost.cmake)

add_subdirectory(core)
add_subdirectory(rest)
add_subdirectory(zoo)

set(SOURCE_FILES
    ${REST_SOURCE_FILES}
    ${SERVICES_SOURCE_FILES}
    ${ZOO_SOURCE_FILES}
    main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

include(Feature.cmake)

add_library(boost INTERFACE)

target_include_directories(
    boost
    INTERFACE
        ${Boost_ROOT}
)

target_link_libraries(
    boost
    INTERFACE
        ${Boost_LIBRARIES}
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${Boost_ROOT}
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
        rest
        zoo
        core
        boost
)

if(ENABLE_UNIT_TESTS)
    add_subdirectory(tests)
endif()
