#include "server/protocol.h"

namespace seka::server {

boost::json::object make_room_state(const Room& room) {
    boost::json::array players;
    for (const auto& player : room.players) {
        boost::json::array cards;
        for (const auto& card : player.hand.cards()) {
            cards.push_back(card.to_string());
        }
        boost::json::object entry;
        entry["id"] = player.id;
        entry["name"] = player.name;
        entry["status"] = static_cast<int>(player.status);
        entry["cards"] = cards;
        players.push_back(entry);
    }

    boost::json::object room_json;
    room_json["roomId"] = room.id;
    room_json["code"] = room.code;
    room_json["state"] = static_cast<int>(room.state);
    room_json["seedHash"] = room.seed_hash;
    room_json["seedReveal"] = room.revealed_seed;
    room_json["players"] = players;
    return room_json;
}

boost::json::object make_tournament_state(const Tournament& tournament) {
    boost::json::array participants;
    for (const auto& player : tournament.participants) {
        boost::json::object entry;
        entry["id"] = player.id;
        entry["name"] = player.name;
        entry["status"] = static_cast<int>(player.status);
        participants.push_back(entry);
    }

    boost::json::object obj;
    obj["tournamentId"] = tournament.id;
    obj["state"] = static_cast<int>(tournament.state);
    obj["winnerId"] = tournament.winner_id;
    obj["participants"] = participants;
    return obj;
}

}  // namespace seka::server
