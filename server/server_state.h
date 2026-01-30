#ifndef SEKA_SERVER_SERVER_STATE_H
#define SEKA_SERVER_SERVER_STATE_H

#include <unordered_map>

#include "core/secure_rng.h"
#include "server/room_manager.h"
#include "server/tournament_manager.h"

namespace seka::server {

class ServerState {
  public:
    explicit ServerState(core::SecureRng& rng);

    RoomManager& rooms();
    TournamentManager& tournaments();

  private:
    RoomManager room_manager_;
    TournamentManager tournament_manager_;
};

}  // namespace seka::server

#endif  // SEKA_SERVER_SERVER_STATE_H
