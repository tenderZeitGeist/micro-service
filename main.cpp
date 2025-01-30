#include <core/logger/Logging.hpp>
#include <rest/Alias.hpp>
#include <rest/Connection.hpp>
#include <rest/Controller.hpp>
#include <zoo/database/InMemoryDatabase.hpp>
#include <zoo/services/AnimalRepository.hpp>
#include <zoo/services/AnimalServiceController.hpp>
#include <zoo/services/CompoundRepository.hpp>

int main() {
    if constexpr (ENABLE_LOGGING) {
        core::logger::start();
        core::logger::log("Start logging...");
    }
    if constexpr (ENABLE_PERFORMANCE_LOGGING) {
        core::logger::log("Enabled performance logging.");
    }
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
