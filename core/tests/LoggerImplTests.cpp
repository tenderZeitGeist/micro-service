
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <core/Queue.hpp>
#include <core/LoggerInterface.hpp>


namespace {
    class LoggerMock
        : public LoggerInterface {
        public:
            explicit LoggerMock(std::condition_variable& cv)
                : m_cv(cv) {
                ON_CALL(*this, log(::testing::_)).WillByDefault([this](const std::string& message){ m_cv.notify_all(); });
            }
        MOCK_METHOD(void, log, (const std::string&), (const, override));
        private:
            std::condition_variable& m_cv;
    };
}

class LoggerImplTestFixture
    : public testing::Test {
protected:
    explicit LoggerImplTestFixture()
        : m_loggerMock(m_cv){

    }

    core::logger::Queue m_loggerQueue;
    std::condition_variable m_cv;
    LoggerMock m_loggerMock;
};

TEST_F(LoggerImplTestFixture, initial_state) {
    EXPECT_FALSE(m_loggerQueue.running());
}

TEST_F(LoggerImplTestFixture, start_queue) {
    m_loggerQueue.start();
    EXPECT_TRUE(m_loggerQueue.running());
}

TEST_F(LoggerImplTestFixture, stop_queue) {
    m_loggerQueue.start();
    ASSERT_TRUE(m_loggerQueue.running());
    m_loggerQueue.stop();
    EXPECT_FALSE(m_loggerQueue.running());
}

TEST_F(LoggerImplTestFixture, queue_message) {
    m_loggerQueue.queue("foo");
    EXPECT_FALSE(m_loggerQueue.isEmpty());
    m_loggerQueue.start();
    m_loggerQueue.queue("bar");


    EXPECT_TRUE(m_loggerQueue.isEmpty());
}
