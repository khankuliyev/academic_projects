#include "server/server.h"

#include <boost/beast/core.hpp>

#include "core/secure_rng.h"
#include "server/session.h"

namespace seka::server {

Server::Server(boost::asio::io_context& ioc, const boost::asio::ip::tcp::endpoint& endpoint)
    : ioc_(ioc),
      acceptor_(boost::asio::make_strand(ioc)),
      rng_(core::create_platform_rng()),
      state_(*rng_) {
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::socket_base::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen(boost::asio::socket_base::max_listen_connections);
}

void Server::run() {
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(boost::asio::make_strand(ioc_),
                           boost::beast::bind_front_handler(&Server::on_accept, this));
}

void Server::on_accept(boost::beast::error_code ec, boost::asio::ip::tcp::socket socket) {
    if (!ec) {
        std::make_shared<Session>(boost::beast::tcp_stream(std::move(socket)), state_, *rng_)->run();
    }
    do_accept();
}

}  // namespace seka::server
