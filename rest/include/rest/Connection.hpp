//
// Created by zeitgeist on 08.04.24.
//

#pragma once

#include "Alias.hpp"

#include <boost/asio/signal_set.hpp>

#include <memory>
#include <thread>
#include <vector>

namespace net = boost::asio;

namespace rest {
    class Controller;
    class HttpWorker;
    struct Route;
}

namespace rest {

    class Connection {
    public:
        explicit Connection(net::ip::address host, uint16_t port, std::size_t numOfThreads);
        ~Connection();
        [[nodiscard]] bool connectController(std::reference_wrapper<const Controller> controller);
        void run();

    private:
        net::io_context m_ioContext;
        net::ip::address m_host;
        uint16_t m_port;
        std::size_t m_numOfThreads;
        Tcp::acceptor m_acceptor;

        std::unique_ptr<net::signal_set> m_signals;
        std::vector<HttpWorker> m_workers;
        std::vector<std::thread> m_threads;
    };
}