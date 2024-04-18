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
        explicit HttpWorker(
            std::reference_wrapper<Tcp::acceptor> acceptor,
            std::reference_wrapper<const Controller> controller
        );

        void start();

    private:
        void accept();
        void readRequest();
        void processRequest(const HttpRequest& request);
        void send();
        void prepareResponse(http::status status, std::string body, std::string_view mimeType);
        void checkTimeout();

        std::reference_wrapper<Tcp::acceptor> m_acceptor;
        std::reference_wrapper<const Controller> m_controller;
        Tcp::socket m_socket{m_acceptor.get().get_executor()};
        boost::asio::basic_waitable_timer<clock> m_requestTimeout{
            m_acceptor.get().get_executor(),
            clock::time_point::max()
        };
        beast::flat_buffer m_buffer;

        std::unique_ptr<HttpParser> m_parser;
        std::unique_ptr<HttpResponse> m_response;
        std::unique_ptr<HttpSerializer> m_serializer;
    };
}
