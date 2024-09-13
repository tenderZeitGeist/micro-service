project(core-tests)

set(CORE_TEST_FILES
    "${CMAKE_CURRENT_LIST_DIR}/MessageQueueTests.cpp"
)

add_executable(
        ${PROJECT_NAME}
        ${CORE_TEST_FILES}
)

target_link_libraries(
        ${PROJECT_NAME}
        core
        gtest
        gmock
        gtest_main
)

target_include_directories(
        ${PROJECT_NAME}
        PRIVATE
            ${DIR_PRIVATE}
)
