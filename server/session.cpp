#include "server/session.h"

#include <boost/json.hpp>

#include "server/protocol.h"
#include "server/random_util.h"

namespace seka::server {

Session::Session(boost::beast::tcp_stream stream, ServerState& state, core::SecureRng& rng)
    : ws_(std::move(stream)), state_(state), rng_(rng) {}

void Session::run() {
    ws_.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));
    ws_.set_option(boost::beast::websocket::stream_base::decorator(
        [](boost::beast::websocket::response_type& res) {
            res.set(boost::beast::http::field::server, "SekaServer");
        }));

    ws_.async_accept(boost::beast::bind_front_handler(&Session::on_accept, shared_from_this()));
}

void Session::on_accept(boost::beast::error_code ec) {
    if (ec) {
        return;
    }

    session_id_ = random_hex(rng_, 8);
    boost::json::object hello;
    hello["type"] = "welcome";
    hello["sessionId"] = session_id_;
    send_json(hello);
    do_read();
}

void Session::do_read() {
    ws_.async_read(buffer_, boost::beast::bind_front_handler(&Session::on_read, shared_from_this()));
}

void Session::on_read(boost::beast::error_code ec, std::size_t) {
    if (ec == boost::beast::websocket::error::closed) {
        state_.rooms().mark_disconnected(session_id_);
        return;
    }
    if (ec) {
        state_.rooms().mark_disconnected(session_id_);
        return;
    }

    std::string payload = boost::beast::buffers_to_string(buffer_.data());
    buffer_.consume(buffer_.size());
    handle_message(payload);
    do_read();
}

void Session::handle_message(const std::string& payload) {
    boost::json::error_code ec;
    auto value = boost::json::parse(payload, ec);
    if (ec || !value.is_object()) {
        return;
    }
    auto obj = value.as_object();
    std::string type = boost::json::value_to<std::string>(obj["type"]);

    state_.rooms().start_expired_rooms();

    auto room_for_player = state_.rooms().find_by_player(session_id_);
    if (room_for_player && room_for_player->get().state == RoomState::Playing
        && room_for_player->get().revealed_seed.empty() && !room_for_player->get().seed_hash.empty()) {
        boost::json::object commit;
        commit["type"] = "seed_commitment";
        commit["seedHash"] = room_for_player->get().seed_hash;
        send_json(commit);

        state_.rooms().deal_cards(room_for_player->get());
        boost::json::object started;
        started["type"] = "room_started";
        started["room"] = make_room_state(room_for_player->get());
        send_json(started);
    }

    if (type == "reconnect") {
        session_id_ = boost::json::value_to<std::string>(obj["sessionId"]);
        state_.rooms().reconnect_player(session_id_);
        boost::json::object response;
        response["type"] = "reconnected";
        response["sessionId"] = session_id_;
        send_json(response);
        return;
    }

    std::string name = obj.if_contains("name") ? boost::json::value_to<std::string>(obj["name"]) : "Player";

    if (type == "quick_match") {
        auto& room = state_.rooms().quick_match(session_id_, name);
        boost::json::object response;
        response["type"] = "room_joined";
        response["room"] = make_room_state(room);
        send_json(response);
        if (room.players.size() >= 2 && state_.rooms().start_room(room)) {
            boost::json::object commit;
            commit["type"] = "seed_commitment";
            commit["seedHash"] = room.seed_hash;
            send_json(commit);

            state_.rooms().deal_cards(room);
            boost::json::object started;
            started["type"] = "room_started";
            started["room"] = make_room_state(room);
            send_json(started);
        } else if (room.state == RoomState::Playing && room.revealed_seed.empty() && !room.seed_hash.empty()) {
            boost::json::object commit;
            commit["type"] = "seed_commitment";
            commit["seedHash"] = room.seed_hash;
            send_json(commit);

            state_.rooms().deal_cards(room);
            boost::json::object started;
            started["type"] = "room_started";
            started["room"] = make_room_state(room);
            send_json(started);
        }
        return;
    }

    if (type == "create_private") {
        auto& room = state_.rooms().create_private(session_id_, name);
        boost::json::object response;
        response["type"] = "room_created";
        response["room"] = make_room_state(room);
        send_json(response);
        return;
    }

    if (type == "join_private") {
        std::string code = boost::json::value_to<std::string>(obj["code"]);
        auto room_opt = state_.rooms().join_private(code, session_id_, name);
        boost::json::object response;
        if (room_opt) {
            response["type"] = "room_joined";
            response["room"] = make_room_state(room_opt->get());
        } else {
            response["type"] = "error";
            response["message"] = "Room not found or full.";
        }
        send_json(response);
        return;
    }

    if (type == "start_room") {
        std::string room_id = boost::json::value_to<std::string>(obj["roomId"]);
        auto room_opt = state_.rooms().find_room(room_id);
        boost::json::object response;
        if (room_opt && state_.rooms().start_room(room_opt->get())) {
            boost::json::object commit;
            commit["type"] = "seed_commitment";
            commit["seedHash"] = room_opt->get().seed_hash;
            send_json(commit);

            state_.rooms().deal_cards(room_opt->get());
            response["type"] = "room_started";
            response["room"] = make_room_state(room_opt->get());
        } else {
            response["type"] = "error";
            response["message"] = "Unable to start room.";
        }
        send_json(response);
        return;
    }

    if (type == "create_tournament") {
        auto& tournament = state_.tournaments().create_tournament();
        state_.tournaments().join_tournament(tournament.id, session_id_, name);
        boost::json::object response;
        response["type"] = "tournament_created";
        response["tournament"] = make_tournament_state(tournament);
        send_json(response);
        return;
    }

    if (type == "join_tournament") {
        std::string tournament_id = boost::json::value_to<std::string>(obj["tournamentId"]);
        boost::json::object response;
        if (state_.tournaments().join_tournament(tournament_id, session_id_, name)) {
            auto tournament = state_.tournaments().find_tournament(tournament_id);
            response["type"] = "tournament_joined";
            response["tournament"] = make_tournament_state(tournament->get());
        } else {
            response["type"] = "error";
            response["message"] = "Unable to join tournament.";
        }
        send_json(response);
        return;
    }

    if (type == "start_tournament") {
        std::string tournament_id = boost::json::value_to<std::string>(obj["tournamentId"]);
        auto tournament_opt = state_.tournaments().find_tournament(tournament_id);
        boost::json::object response;
        if (tournament_opt && state_.tournaments().start_tournament(tournament_opt->get())) {
            response["type"] = "tournament_round_started";
            response["tournament"] = make_tournament_state(tournament_opt->get());
        } else {
            response["type"] = "error";
            response["message"] = "Unable to start tournament.";
        }
        send_json(response);
        return;
    }

    if (type == "advance_tournament") {
        std::string tournament_id = boost::json::value_to<std::string>(obj["tournamentId"]);
        auto tournament_opt = state_.tournaments().find_tournament(tournament_id);
        if (tournament_opt) {
            state_.tournaments().advance_round(tournament_opt->get());
            boost::json::object response;
            if (tournament_opt->get().state == TournamentState::Finished) {
                response["type"] = "tournament_finished";
            } else {
                response["type"] = "tournament_round_started";
            }
            response["tournament"] = make_tournament_state(tournament_opt->get());
            send_json(response);
        }
        return;
    }
}

void Session::send_json(const boost::json::object& obj) {
    auto text = boost::json::serialize(obj);
    ws_.text(true);
    ws_.async_write(boost::asio::buffer(text),
                    [self = shared_from_this()](boost::beast::error_code, std::size_t) {});
}

}  // namespace seka::server
