cmake_minimum_required(VERSION 3.27)
project(micro_services)

set(CMAKE_CXX_STANDARD 20)
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -Wall -Wextra -Og -Wconversion -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
message(STATUS "Debug flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Release flags: ${CMAKE_CXX_FLAGS_RELEASE}")

option(USE_SYSTEM_BOOST "Use system boost" Off)

find_package(Boost QUIET)

if(BOOST_FOUND AND USE_SYSTEM_BOOST)
	message(STATUS "Using system boost library")
else()
### Boost 1.84 ###
	set(BOOST_ROOT "${LIB_DIR}/boost_1_84_0")
	set(BOOST_INCLUDE_DIR "${BOOST_ROOT}/include")
	set(BOOST_LIBRARY_DIR "${BOOST_ROOT}/lib")
	find_package(Boost REQUIRED)
	message(STATUS "Using system boost library")
endif()
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIR}")
message(STATUS "Boost lib dirs: ${Boost_LIBRARY_DIR}")
message(STATUS "Boost libs: ${Boost_LIBRARIES}")

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(
	${PROJECT_NAME}
	${Boost_LIBRARIES}
)