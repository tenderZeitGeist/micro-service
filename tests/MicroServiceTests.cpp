//
// Created by abentler on 16.04.24.
//

#include <gmock/internal/gmock-internal-utils.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <core/database/Entity.hpp>
#include <rest/Connection.hpp>
#include <rest/Controller.hpp>
#include <rest/Alias.hpp>
#include <zoo/animal/Animal.hpp>
#include <zoo/compound/Compound.hpp>
#include <zoo/services/AnimalServiceController.hpp>
#include <zoo/services/AnimalRepository.hpp>
#include <zoo/services/CompoundRepository.hpp>

#include <curl/curl.h>

namespace {

size_t writeCallback(void* contents, size_t size, size_t numOfBytes, std::string* buffer) {
    const auto n = size * numOfBytes;
    buffer->append(static_cast<char*>(contents), n);
    return n;
}

using namespace core::database;

class MockDatabaseInterface
    : public DatabaseInterface {
public:
    MOCK_METHOD(std::shared_ptr<Entity>, getEntityById, (std::size_t), (override ));
    MOCK_METHOD(std::vector<std::shared_ptr<Entity>>, getAllEntities, (), (override));
    MOCK_METHOD(bool, deleteEntity, (std::shared_ptr<Entity>), (override));
    MOCK_METHOD(std::size_t, addEntity, (std::shared_ptr<Entity>), (override));
    MOCK_METHOD(std::vector<std::size_t>, addEntities, (std::vector<std::shared_ptr<Entity>>), (override));
};

}

class MicroServiceTestSuite
    : public ::testing::Test {
protected:
    MicroServiceTestSuite() {
        const auto database = std::make_shared<MockDatabaseInterface>();
        m_databaseMock = database.get();
        auto serviceController = std::make_unique<zoo::AnimalServiceController>(
            std::make_unique<zoo::AnimalRepository>(database),
            std::make_unique<zoo::CompoundRepository>(database)
        );
        constexpr auto numOfThreads = 1u;
        m_controller = std::make_unique<rest::Controller>(std::move(serviceController));
        m_connection = std::make_unique<rest::Connection>(net::ip::make_address(rest::kDefaultAddress), rest::kDefaultPort, numOfThreads);
    }

    void TearDown() override {
        std::raise(SIGINT);
        if (m_mainConnectionThread.joinable()) {
            m_mainConnectionThread.join();
        }
        curl_global_cleanup();
    }

    void SetUp() override {
        curl_global_init(CURL_GLOBAL_ALL);
        ASSERT_TRUE(m_connection->connectController(*m_controller));
        m_mainConnectionThread = std::thread([connection = m_connection.get()]{ connection->run(); });
    }

    void performGetRequest() {
        CURL* curl = curl_easy_init();
        ASSERT_TRUE(curl);
        curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/compounds");
        std::string responseBuffer;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);
        const CURLcode res = curl_easy_perform(curl);
        EXPECT_EQ(res, CURLE_OK);
        std::cerr << responseBuffer << '\n';
    }

    std::unique_ptr<rest::Controller> m_controller;
    std::unique_ptr<rest::Connection> m_connection;
    std::thread m_mainConnectionThread;
    MockDatabaseInterface* m_databaseMock;
};

TEST_F(MicroServiceTestSuite, get_compounds) {
    std::vector<std::shared_ptr<Entity>> compoundList({std::make_shared<zoo::Compound>("foo", 42)});
    EXPECT_CALL(*m_databaseMock, getAllEntities()).WillRepeatedly(::testing::Return(compoundList));
    performGetRequest();
}
