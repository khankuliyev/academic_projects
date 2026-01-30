#ifndef SEKA_SERVER_MODELS_H
#define SEKA_SERVER_MODELS_H

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

#include "core/hand.h"

namespace seka::server {

enum class PlayerStatus {
    Connected,
    Disconnected,
    Eliminated
};

struct PlayerState {
    std::string id;
    std::string name;
    PlayerStatus status{PlayerStatus::Connected};
    core::Hand hand;
};

enum class RoomType {
    Random,
    Private
};

enum class RoomState {
    Waiting,
    Playing,
    Finished
};

struct Room {
    std::string id;
    std::string code;
    RoomType type{RoomType::Random};
    RoomState state{RoomState::Waiting};
    std::vector<PlayerState> players;
    std::string seed_hash;
    std::string revealed_seed;
    std::vector<std::uint8_t> seed_bytes;
    std::chrono::steady_clock::time_point created_at{std::chrono::steady_clock::now()};
};

enum class TournamentState {
    Registering,
    Running,
    Finished
};

struct Tournament {
    std::string id;
    TournamentState state{TournamentState::Registering};
    std::vector<PlayerState> participants;
    std::vector<Room> tables;
    std::string winner_id;
};

}  // namespace seka::server

#endif  // SEKA_SERVER_MODELS_H
