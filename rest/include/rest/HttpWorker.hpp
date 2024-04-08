//
// Created by zeitgeist on 08.04.24.
//
#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

#include "rest/Alias.hpp"
#include "rest/Controller.hpp"

namespace rest {

    class HttpWorker {
        using clock = std::chrono::steady_clock;
        static constexpr std::chrono::seconds kTimeout{60};
    public:
        HttpWorker(
            std::reference_wrapper<tcp::acceptor> acceptor,
            std::reference_wrapper<Controller> controller
        );

        void start();

    private:
        void accept();
        void readRequest();
        void processRequest(const http_request& request);
        void send();
        void prepareResponse(http::status status, const std::string& body, std::string_view mimeType);
        void sendError(http::status status, const std::string& error, std::string_view mimeType);
        void checkTimeout();

        std::reference_wrapper<tcp::acceptor> m_acceptor;
        std::reference_wrapper<Controller> m_controller;
        tcp::socket m_socket{m_acceptor.get().get_executor()};
        boost::asio::basic_waitable_timer<clock> m_requestTimeout{
            m_acceptor.get().get_executor(),
            clock::time_point::max()
        };
        beast::flat_buffer m_buffer;

        std::optional<http_parser> m_parser;
        std::optional<http_response> m_response;
        std::optional<http_serializer> m_serializer;
    };
}
