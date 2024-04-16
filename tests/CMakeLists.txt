set(EXECUTABLE micro-service-tests)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(DIR_SOURCE ${CMAKE_CURRENT_LIST_DIR})

set(
    MICRO_SERVICE_TEST_FILES
    "${DIR_SOURCE}/MicroServiceTests.cpp"
)

find_package(CURL REQUIRED)

enable_testing()

add_executable(
    ${EXECUTABLE}
    ${MICRO_SERVICE_TEST_FILES}
)

target_link_libraries(
    ${EXECUTABLE}
    core
    rest
    zoo
    curl

    gtest
    gmock
    gtest_main
)
