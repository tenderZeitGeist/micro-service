
#include <gtest/gtest.h>

#include <core/LoggerImpl.hpp>

class LoggerImplTestFixture
    : public testing::Test {
protected:
    explicit LoggerImplTestFixture() {

    }

    ~LoggerImplTestFixture() {
        m_logger.stop();
    }

    core::logger::LoggerImpl m_logger;
};

TEST_F(LoggerImplTestFixture, initial_state) {
    EXPECT_FALSE(m_logger.running());
}

TEST_F(LoggerImplTestFixture, start_queue) {
    m_logger.start();
    EXPECT_TRUE(m_logger.running());
}

TEST_F(LoggerImplTestFixture, )