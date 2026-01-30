#include "server/server_state.h"

namespace seka::server {

ServerState::ServerState(core::SecureRng& rng) : room_manager_(rng), tournament_manager_(rng) {}

RoomManager& ServerState::rooms() {
    return room_manager_;
}

TournamentManager& ServerState::tournaments() {
    return tournament_manager_;
}

}  // namespace seka::server
