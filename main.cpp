#include <iostream>

#include <rest/Alias.hpp>
#include <rest/Controller.hpp>
#include <rest/Connection.hpp>

#include <zoo/services/AnimalService.hpp>
#include <zoo/services/CompoundService.hpp>
#include <zoo/services/ServiceController.hpp>
#include <zoo/database/InMemoryDatabase.hpp>

constexpr std::size_t kMaxThreads = 4;

int main() {
    std::shared_ptr<DatabaseInterface> database = std::make_shared<InMemoryDatabase>();
    std::shared_ptr<zoo::ServiceControllerInterface> serviceController = std::make_unique<zoo::ServiceController>(
        std::make_unique<zoo::AnimalService>(database),
        std::make_unique<zoo::CompoundService>(database)
    );
    const auto numOfThreads = std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()), kMaxThreads);
    rest::Connection connection(net::ip::make_address(rest::kDefaultAddress), rest::kDefaultPort, numOfThreads);
    const auto controller = connection.initializeController({
        {
            rest::Method::get,
            rest::Endpoint("/compounds"),
            [serviceController](const rest::HttpRequest&) { return serviceController->getAllCompounds(); }
        },
        {
            rest::Method::get,
            rest::Endpoint("/compounds/[a-zA-Z]{1,255}$"),
            [serviceController](const rest::HttpRequest& r) { return serviceController->getCompoundByName(r); }
        },
        {
            rest::Method::get,
            rest::Endpoint("/animals/[a-zA-Z]{1,255}$"),
            [serviceController](const rest::HttpRequest& r) { return serviceController->getAnimalByName(r); }
        },
        {
            rest::Method::post,
            rest::Endpoint("/compounds/[a-zA-Z]{1,255}$"),
            [serviceController](const rest::HttpRequest& r) { return serviceController->addAnimalToCompound(r); }
        },
        {
            rest::Method::delete_,
            rest::Endpoint("/compounds/[a-zA-Z]{1,255}/animal/[a-zA-Z]{1,255}$"),
            [serviceController](const rest::HttpRequest& r) { return serviceController->deleteAnimalFromCompound(r); }
        },
        {
            rest::Method::get,
            rest::Endpoint("/animal/(?i:mammal|fish|bird|reptile)"),
            [serviceController](const rest::HttpRequest& r) { return serviceController->getAllAnimalsBySpecies(r); }
        },
    });
    connection.run();
    return 0;
}
