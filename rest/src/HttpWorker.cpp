//
// Created by zeitgeist on 08.04.24.
//

#include "rest/Alias.hpp"
#include "rest/HttpWorker.hpp"

#include <iostream>

namespace rest {

    HttpWorker::HttpWorker(std::reference_wrapper<Tcp::acceptor> acceptor,
                                 std::reference_wrapper<const Controller> controller)
        : m_acceptor(acceptor)
        , m_controller(controller){

    }

    void HttpWorker::start() {
        accept();
        checkTimeout();
    }

    void HttpWorker::accept() {
        beast::error_code error;
        [[maybe_unused]]const auto _ = m_socket.close(error);
        m_buffer.consume(m_buffer.size());
        m_acceptor.get().async_accept(
            m_socket,
            [this](beast::error_code error){
                if(error) {
                    accept();
                } else {
                    readRequest();
                }
            });
    }

    void HttpWorker::readRequest() {
        m_parser = std::make_unique<HttpParser>();
        http::async_read(
            m_socket,
            m_buffer,
            *m_parser,
            [this](beast::error_code error, std::size_t) {
                if(error) {
                    accept();
                } else {
                    processRequest(m_parser->get());
                }
            });
    }

    void HttpWorker::processRequest(const HttpRequest& request) {
        auto [status, body, mimeType] = m_controller.get().execute(request);
        prepareResponse(status, std::move(body), mimeType);
    }

    void HttpWorker::send() {
        m_serializer = std::make_unique<HttpSerializer>(*m_response);
        http::async_write(
            m_socket,
            *m_serializer,
            [this](beast::error_code ec, std::size_t) {
                [[maybe_unused]]const auto _ = m_socket.shutdown(Tcp::socket::shutdown_send, ec);
                accept();
            });
    }

    void HttpWorker::prepareResponse(http::status status, std::string body, std::string_view mimeType) {
        m_response = std::make_unique<HttpResponse>();
        m_response->result(status);
        m_response->body() = std::move(body);
        m_response->set(http::field::content_type, mimeType);
        m_response->set(http::field::server, kServerInfo);
        m_response->set(http::field::access_control_allow_origin, "");
        m_response->prepare_payload();
        send();
    }

    void HttpWorker::checkTimeout() {
        if(m_requestTimeout.expiry() <= clock::now()) {
            m_socket.close();
            m_requestTimeout.expires_at(clock::time_point::max());
        }
        m_requestTimeout.async_wait([this](beast::error_code) { checkTimeout();});
    }
}
