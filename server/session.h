#ifndef SEKA_SERVER_SESSION_H
#define SEKA_SERVER_SESSION_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <memory>
#include <string>

#include "core/secure_rng.h"
#include "server/server_state.h"

namespace seka::server {

class Session : public std::enable_shared_from_this<Session> {
  public:
    Session(boost::beast::tcp_stream stream, ServerState& state, core::SecureRng& rng);

    void run();

  private:
    boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
    boost::beast::flat_buffer buffer_;
    ServerState& state_;
    core::SecureRng& rng_;
    std::string session_id_;

    void on_accept(boost::beast::error_code ec);
    void do_read();
    void on_read(boost::beast::error_code ec, std::size_t bytes);
    void handle_message(const std::string& payload);
    void send_json(const boost::json::object& obj);
};

}  // namespace seka::server

#endif  // SEKA_SERVER_SESSION_H
