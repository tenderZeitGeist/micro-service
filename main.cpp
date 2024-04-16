#include <iostream>

#include <rest/Alias.hpp>
#include <rest/Controller.hpp>
#include <rest/Connection.hpp>

#include <zoo/services/AnimalRepository.hpp>
#include <zoo/services/CompoundRepository.hpp>
#include <zoo/services/AnimalServiceController.hpp>
#include <zoo/database/InMemoryDatabase.hpp>

int main() {
    const auto database = std::make_shared<zoo::database::InMemoryDatabase>();
    auto serviceController = std::make_unique<zoo::AnimalServiceController>(
        std::make_unique<zoo::AnimalRepository>(database),
        std::make_unique<zoo::CompoundRepository>(database)
    );
    const auto numOfThreads = std::min(static_cast<std::size_t>(std::thread::hardware_concurrency()), rest::kMaxThreads);
    const rest::Controller controller(std::move(serviceController));
    rest::Connection connection(net::ip::make_address(rest::kDefaultAddress), rest::kDefaultPort, numOfThreads);
    if(!connection.connectController(std::cref(controller))) {
        std::abort();
    }
    connection.run();
    return 0;
}
