cmake_minimum_required(VERSION 3.25)

project(micro-service)

set(CMAKE_CXX_STANDARD 20)
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -Wall -Wextra -Og -Wconversion -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
message(STATUS "Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")

option(ENABLE_UNIT_TESTS "Enables unit tests" Off)

find_package(Boost REQUIRED COMPONENTS url json)
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIR}")
message(STATUS "Boost lib dirs: ${Boost_LIBRARY_DIR}")
message(STATUS "Boost libs: ${Boost_LIBRARIES}")

add_subdirectory(core)
add_subdirectory(rest)
add_subdirectory(zoo)

set(SOURCE_FILES
    ${REST_SOURCE_FILES}
    ${ZOO_SOURCE_FILES}
    main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    rest
    zoo
    core
    ${Boost_LIBRARIES}
)

if(ENABLE_UNIT_TESTS)
    add_subdirectory(tests)
endif()
