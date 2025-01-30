
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <core/LoggerInterface.hpp>
#include <core/MessageQueue.hpp>


namespace {

class LoggerMock : public core::logger::LoggerInterface {
public:
    MOCK_METHOD(void, log, (const std::string&), (const, override));
};

}

class MessageQueueTestFixture : public testing::Test {
protected:
    explicit MessageQueueTestFixture() {
        auto logger = std::make_unique<LoggerMock>();
        m_loggerMock = logger.get();
        m_loggerQueue = std::make_unique<core::logger::MessageQueue>(std::move(logger));
    }

    std::unique_ptr<core::logger::MessageQueue> m_loggerQueue;
    std::condition_variable m_cv;
    std::mutex m_mutex;
    LoggerMock* m_loggerMock;
};

TEST_F(MessageQueueTestFixture, initial_state) {
    EXPECT_FALSE(m_loggerQueue->running());
}

TEST_F(MessageQueueTestFixture, start_queue) {
    m_loggerQueue->start();
    EXPECT_TRUE(m_loggerQueue->running());
}

TEST_F(MessageQueueTestFixture, stop_queue) {
    m_loggerQueue->start();
    ASSERT_TRUE(m_loggerQueue->running());
    m_loggerQueue->stop();
    EXPECT_FALSE(m_loggerQueue->running());
}

TEST_F(MessageQueueTestFixture, queue_message) {
    m_loggerQueue->start();
    const std::string message = "bar";

    EXPECT_CALL(*m_loggerMock, log(message)).WillOnce([this](const std::string&) { m_cv.notify_all(); });
    {
        using namespace std::chrono_literals;
        std::unique_lock lk{m_mutex};
        m_loggerQueue->queue(message);
        m_cv.wait_for(lk, 10ms);
    }
    EXPECT_TRUE(m_loggerQueue->isEmpty());
}

TEST_F(MessageQueueTestFixture, queue_messages) {
    m_loggerQueue->start();
    const std::string message = "bar";
    constexpr std::size_t numOfCalls = 10;

    EXPECT_CALL(*m_loggerMock, log(::testing::_)).Times(numOfCalls);
    for (std::size_t i = 0; i < numOfCalls; ++i) {
        m_loggerQueue->queue(std::to_string(i));
    }

    EXPECT_CALL(*m_loggerMock, log(message)).WillOnce([this](const std::string&) { m_cv.notify_all(); });
    {
        using namespace std::chrono_literals;
        std::unique_lock lk{m_mutex};
        m_loggerQueue->queue(message);
        m_cv.wait_for(lk, 10ms);
    }
    EXPECT_TRUE(m_loggerQueue->isEmpty());
}
