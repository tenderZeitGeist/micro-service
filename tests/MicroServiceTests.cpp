//
// Created by abentler on 16.04.24.
//

#include <boost/json.hpp>
#include <curl/curl.h>
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
    MockDatabaseInterface() {
        auto animal = std::make_shared<zoo::Animal>("Foo", 42, "Mammal");
        setEntityId(animal.get());
        auto compound = std::make_shared<zoo::Compound>("Foo", 42);
        if (!compound->addAnimal(animal->getId())) {
            assert(false);
        }
        setEntityId(compound.get());
        const auto animalId = animal->getId();
        const auto compoundId = compound->getId();
        m_entities.emplace(animalId, std::move(animal));
        m_entities.emplace(compoundId, std::move(compound));

        ON_CALL(*this, getAllEntities()).WillByDefault(
            [this]() -> std::vector<std::shared_ptr<Entity>> {
                auto view = m_entities | std::views::values | std::views::common;
                return {view.begin(), view.end()};
            }
        );

        ON_CALL(*this, getEntityById(::testing::_)).WillByDefault(
            [this](std::size_t id) -> std::shared_ptr<Entity> {
                if (const auto iter = m_entities.find(id); iter != m_entities.end()) {
                    return iter->second;
                }
                return {};
            }
        );

        ON_CALL(*this, deleteEntity(::testing::_)).WillByDefault(
            [this](std::shared_ptr<Entity> entity) -> bool {
                const auto id = entity->getId();
                if (m_entities.contains(id)) {
                    m_entities.erase(id);
                    return true;
                }
                return false;
            }
        );

        ON_CALL(*this, addEntity(::testing::_)).WillByDefault(
            [this](std::shared_ptr<Entity> entity) -> std::size_t {
                setEntityId(entity.get());
                const auto id = entity->getId();
                m_entities.emplace(id, std::move(entity));
                return id;
            }
        );
    }

    MOCK_METHOD(std::shared_ptr<Entity>, getEntityById, (std::size_t), (override ));
    MOCK_METHOD(std::vector<std::shared_ptr<Entity>>, getAllEntities, (), (override));
    MOCK_METHOD(bool, deleteEntity, (std::shared_ptr<Entity>), (override));
    MOCK_METHOD(std::size_t, addEntity, (std::shared_ptr<Entity>), (override));
    MOCK_METHOD(std::vector<std::size_t>, addEntities, (std::vector<std::shared_ptr<Entity>>), (override));

private:
    std::unordered_map<std::size_t, std::shared_ptr<Entity>> m_entities;
};

}

class MicroServiceTestSuite
    : public ::testing::Test {
protected:
    MicroServiceTestSuite() {
        const auto database = std::make_shared<testing::NiceMock<MockDatabaseInterface>>();
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
        m_mainConnectionThread = std::thread(
            [connection = m_connection.get()] {
                connection->run();
            }
        );
    }

    long getResponseInfo(CURL* curl) {
        long httpStatus = -1;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus);
        return httpStatus;
    }

    void performGetRequest(std::string_view url) {
        CURL* curl = curl_easy_init();
        ASSERT_TRUE(curl);

        curl_easy_setopt(curl, CURLOPT_URL, url.data());
        std::string responseBuffer;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

        const CURLcode res = curl_easy_perform(curl);
        const auto httpStatus = getResponseInfo(curl);
        EXPECT_EQ(res, CURLE_OK);
        EXPECT_EQ(httpStatus, 200);

        std::cout << responseBuffer << '\n';
        curl_easy_cleanup(curl);
    }

    void performPostRequest(std::string_view url, const std::string& payload) {
        CURL* curl = curl_easy_init();
        ASSERT_TRUE(curl);

        curl_easy_setopt(curl, CURLOPT_URL, url.data());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.size());

        std::string responseBuffer;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

        const CURLcode res = curl_easy_perform(curl);
        const auto httpStatus = getResponseInfo(curl);

        EXPECT_EQ(res, CURLE_OK);
        EXPECT_EQ(httpStatus, 200);

        std::cout << responseBuffer << '\n';
        curl_easy_cleanup(curl);
    }

    void performDeleteRequest(std::string_view url) {
        CURL* curl = curl_easy_init();
        ASSERT_TRUE(curl);

        curl_easy_setopt(curl, CURLOPT_URL, url.data());
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

        std::string responseBuffer;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);

        const CURLcode res = curl_easy_perform(curl);
        const auto httpStatus = getResponseInfo(curl);
        EXPECT_EQ(res, CURLE_OK);
        EXPECT_EQ(httpStatus, 204);

        std::cout << responseBuffer << '\n';
        curl_easy_cleanup(curl);
    }

    std::unique_ptr<rest::Controller> m_controller;
    std::unique_ptr<rest::Connection> m_connection;
    std::thread m_mainConnectionThread;
    MockDatabaseInterface* m_databaseMock;
};

TEST_F(MicroServiceTestSuite, micro_service_api_integration_test) {
    std::cout << "\nStarting test suite for micro-service app" << "\n";

    for (const auto getRequest: {
             "http://localhost:8080/compounds",
             "http://localhost:8080/compounds/Foo",
             "http://localhost:8080/animals/Foo",
             "http://localhost:8080/animals/species/Mammal"
         }) {
        std::cout << "\nQuerriyng GET request on URL " << getRequest << '\n';
        performGetRequest(getRequest);
    }

    const auto animalPayload = boost::json::serialize(
        boost::json::object{
            {"name", "Bar"},
            {"age", 42},
            {"species", "Bird"}
        }
    );

    constexpr auto postRequest = "http://localhost:8080/compounds/Foo/animals";
    std::cout << "\nAdding animal Foo to compound via POST request on URL " << postRequest << '\n';
    performPostRequest(postRequest, animalPayload);

    constexpr auto deleteRequest = "http://localhost:8080/compounds/Foo/animals/Bar";
    std::cout << "\nDeleting animal Foo via DELETE request on URL " << deleteRequest << '\n';
    performDeleteRequest("http://localhost:8080/compounds/Foo/animals/Bar");

    std::cout << "\nFinished test suite for micro-service app" << "\n";
}
