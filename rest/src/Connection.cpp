//
// Created by zeitgeist on 09.04.24.
//

#include "rest/Connection.hpp"
#include "rest/Controller.hpp"
#include "rest/HttpWorker.hpp"
#include "rest/Route.hpp"

namespace rest {

    Connection::Connection(net::ip::address host, uint16_t port, std::size_t numOfThreads)
        : m_host(std::move(host))
        , m_port(port)
        , m_numOfThreads(numOfThreads)
        , m_acceptor(m_ioContext, {m_host, m_port}) {

    }

    Connection::~Connection() {
        for(auto& thread : m_threads) {
            if(thread.joinable()) {
                thread.join();
            }
        }
    }

    std::unique_ptr<rest::Controller> Connection::initializeController(std::vector<Route> routes) {
        auto controller = std::make_unique<Controller>();
        for(auto& route : routes){
            controller->emplaceRoute(route);
        }

        m_workers.reserve(m_numOfThreads);
        for(std::size_t i = 0; i < m_numOfThreads; ++i) {
            m_workers.emplace_back(m_acceptor, *controller);
            m_workers.back().start();
        }

        m_signals = std::make_unique<net::signal_set>(m_ioContext, SIGINT, SIGTERM);
        m_signals->async_wait([&ioContext = m_ioContext](const boost::system::error_code&, int){ ioContext.stop(); });

        const auto numOfParallelThreads = m_numOfThreads - 1;
        m_threads.reserve(numOfParallelThreads);
        for(std::size_t i = 0; i < numOfParallelThreads; ++i) {
            m_threads.emplace_back([&ioContext = m_ioContext](){ ioContext.run(); });
        }

        return controller;
    }

    void Connection::run() {
        m_ioContext.run();
    }

}
