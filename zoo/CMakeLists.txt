cmake_minimum_required(VERSION 3.5)

project(zoo CXX)

set(DIR_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(DIR_SOURCE ${CMAKE_CURRENT_LIST_DIR}/src)

set(ZOO_HEADER_FILES
        "${DIR_INCLUDE}/zoo/Animal.hpp"
)

set(ZOO_SOURCE_FILES
        "${DIR_SOURCE}/Animal.cpp"
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
