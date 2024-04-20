cmake_minimum_required(VERSION 3.25)

project(micro-service)

set(CMAKE_CXX_STANDARD 20)
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -Wall -Wextra -Og -Wconversion -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
message(STATUS "Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")

option(ENABLE_UNIT_TESTS "Enables unit tests" Off)

set(BOOST_VERSION "1.83.0")
set(BOOST_URL "https://github.com/boostorg/boost/releases/download/boost-${BOOST_VERSION}/boost-${BOOST_VERSION}.tar.gz")
set(BOOST_URL_MD5 "58db882403e0c16b334760f3c3b76ff8")
set(BOOST_LIBS_REQUIRED system beast json)

include(FetchContent)

set(FETCHCONTENT_QUIET FALSE)
FetchContent_Declare(
    Boost
    URL ${BOOST_URL}
    URL_HASH MD5=${BOOST_URL_MD5}
)

FetchContent_MakeAvailable(Boost)

set(Boost_ROOT ${boost_SOURCE_DIR})
message(STATUS "Boost Source Directory: ${boost_SOURCE_DIR}")
message(STATUS "Boost Root: ${Boost_ROOT}")

# find_package(Boost REQUIRED COMPONENTS url json)
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIR}")
message(STATUS "Boost lib dirs: ${Boost_LIBRARY_DIR}")
message(STATUS "Boost libs: ${Boost_LIBRARIES}")

set(Boost_LIBRARIES
    Boost::beast
    Boost::regex
    Boost::url
    Boost::json
)

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

add_library(boost INTERFACE)
target_include_directories(boost INTERFACE ${Boost_ROOT})
target_compile_definitions(boost INTERFACE BOOST_BEAST_USE_STD_STRING_VIEW)
target_link_libraries(boost INTERFACE ${Boost_LIBRARIES})


target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${Boost_ROOT}
)

target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE
        BOOST_BEAST_USE_STD_STRING_VIEW
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
