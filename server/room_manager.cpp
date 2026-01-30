#include "server/room_manager.h"

#include <algorithm>
#include <chrono>

#include "server/random_util.h"
#include "server/table_logic.h"

namespace seka::server {

RoomManager::RoomManager(core::SecureRng& rng) : rng_(rng) {}

Room& RoomManager::quick_match(const std::string& player_id, const std::string& name) {
    for (auto& room : rooms_) {
        if (room.type == RoomType::Random && room.state == RoomState::Waiting && room.players.size() < 7) {
            room.players.push_back({player_id, name, PlayerStatus::Connected});
            if (room.players.size() >= 7) {
                start_room(room);
            } else if (room.players.size() >= 2) {
                auto now = std::chrono::steady_clock::now();
                if (now - room.created_at >= std::chrono::seconds(20)) {
                    start_room(room);
                }
            }
            return room;
        }
    }

    Room room;
    room.id = random_hex(rng_, 8);
    room.type = RoomType::Random;
    room.created_at = std::chrono::steady_clock::now();
    room.players.push_back({player_id, name, PlayerStatus::Connected});
    rooms_.push_back(room);
    return rooms_.back();
}

Room& RoomManager::create_private(const std::string& player_id, const std::string& name) {
    Room room;
    room.id = random_hex(rng_, 8);
    room.code = random_base32(rng_, 6);
    room.type = RoomType::Private;
    room.players.push_back({player_id, name, PlayerStatus::Connected});
    rooms_.push_back(room);
    return rooms_.back();
}

std::optional<std::reference_wrapper<Room>> RoomManager::join_private(const std::string& code,
                                                                      const std::string& player_id,
                                                                      const std::string& name) {
    for (auto& room : rooms_) {
        if (room.type == RoomType::Private && room.code == code && room.state == RoomState::Waiting
            && room.players.size() < 7) {
            room.players.push_back({player_id, name, PlayerStatus::Connected});
            return room;
        }
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<Room>> RoomManager::find_by_player(const std::string& player_id) {
    for (auto& room : rooms_) {
        for (const auto& player : room.players) {
            if (player.id == player_id) {
                return room;
            }
        }
    }
    return std::nullopt;
}

std::optional<std::reference_wrapper<Room>> RoomManager::find_room(const std::string& room_id) {
    for (auto& room : rooms_) {
        if (room.id == room_id) {
            return room;
        }
    }
    return std::nullopt;
}

bool RoomManager::start_room(Room& room) {
    if (room.state != RoomState::Waiting || room.players.size() < 2) {
        return false;
    }
    room.state = RoomState::Playing;
    prepare_seed(room, rng_);
    return true;
}

void RoomManager::deal_cards(Room& room) {
    deal_room(room);
}

void RoomManager::start_expired_rooms() {
    auto now = std::chrono::steady_clock::now();
    for (auto& room : rooms_) {
        if (room.type != RoomType::Random || room.state != RoomState::Waiting) {
            continue;
        }
        if (room.players.size() >= 2 && now - room.created_at >= std::chrono::seconds(20)) {
            start_room(room);
        }
    }
}

void RoomManager::mark_disconnected(const std::string& player_id) {
    auto room_opt = find_by_player(player_id);
    if (!room_opt) {
        return;
    }
    auto& room = room_opt->get();
    for (auto& player : room.players) {
        if (player.id == player_id) {
            player.status = PlayerStatus::Disconnected;
            return;
        }
    }
}

void RoomManager::reconnect_player(const std::string& player_id) {
    auto room_opt = find_by_player(player_id);
    if (!room_opt) {
        return;
    }
    auto& room = room_opt->get();
    for (auto& player : room.players) {
        if (player.id == player_id) {
            player.status = PlayerStatus::Connected;
            return;
        }
    }
}

const std::vector<Room>& RoomManager::rooms() const {
    return rooms_;
}

}  // namespace seka::server
