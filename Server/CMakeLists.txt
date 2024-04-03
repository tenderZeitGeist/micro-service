cmake_minimum_required(VERSION 3.5)

project(Game C CXX)

set(DIR_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(DIR_SOURCE ${CMAKE_CURRENT_LIST_DIR}/src)

set(SERVER_HEADER_FILES
    "${DIR_INCLUDE}/Server/Server.h"
)

set(SERVER_SOURCE_FILES
    "${DIR_SOURCE}/Server.h"
)

add_library(${PROJECT_NAME} STATIC ${SERVER_HEADER_FILES} ${SOURCE_SOURCE_FILES})

# target_link_libraries(${PROJECT_NAME} PRIVATE micro_services)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	${DIR_INCLUDE}
	PRIVATE
	${DIR_SOURCE}
)
