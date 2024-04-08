//
// Created by zeitgeist on 08.04.24.
//
#include "rest/HttpWorker.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
using tcp = boost::asio::ip::tcp;

rest::HttpWorker::HttpWorker(std::reference_wrapper<tcp::acceptor> acceptor,
                             std::reference_wrapper<Controller> controller)
                             : m_acceptor(acceptor)
                             , m_controller(controller){

}

void rest::HttpWorker::start() {
    accept();
    checkTimeout();
}

void rest::HttpWorker::accept() {
    beast::error_code error;
    m_socket.close(error);
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

void rest::HttpWorker::readRequest() {
    m_parser.emplace();
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

void rest::HttpWorker::processRequest(const http_request& request) {
    rest::Response response = m_controller.get().execute(request);
    prepareResponse(response.status, response.body, response.mimeType);
}

void rest::HttpWorker::send() {
    m_response->set(http::field::server, kServerInfo);
    m_response->set(http::field::access_control_allow_origin, "");
    m_response->prepare_payload();
    m_serializer.emplace(*m_response);
    http::async_write(
       m_socket,
       *m_serializer,
       [this](beast::error_code ec, std::size_t) {
           m_socket.shutdown(tcp::socket::shutdown_send, ec);
           m_serializer.reset();
           m_response.reset();
           accept();
       });
}

void rest::HttpWorker::prepareResponse(http::status status, const std::string& body, std::string_view mimeType) {
    m_response.emplace();
    m_response->result(status);
    m_response->body() = body;
    m_response->set(http::field::content_type, mimeType);
    send();
}

void rest::HttpWorker::checkTimeout() {
    if(m_requestTimeout.expiry() <= clock::now()) {
        m_socket.close();
        m_requestTimeout.expires_at(clock::time_point::max());
    }
    m_requestTimeout.async_wait([this](beast::error_code) { checkTimeout();});

}
