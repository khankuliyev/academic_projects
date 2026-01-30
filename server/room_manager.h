#ifndef SEKA_SERVER_ROOM_MANAGER_H
#define SEKA_SERVER_ROOM_MANAGER_H

#include <optional>
#include <unordered_map>

#include "core/secure_rng.h"
#include "server/models.h"

namespace seka::server {

class RoomManager {
  public:
    explicit RoomManager(core::SecureRng& rng);

    Room& quick_match(const std::string& player_id, const std::string& name);
    Room& create_private(const std::string& player_id, const std::string& name);
    std::optional<std::reference_wrapper<Room>> join_private(const std::string& code,
                                                            const std::string& player_id,
                                                            const std::string& name);
    std::optional<std::reference_wrapper<Room>> find_by_player(const std::string& player_id);
    std::optional<std::reference_wrapper<Room>> find_room(const std::string& room_id);

    bool start_room(Room& room);
    void deal_cards(Room& room);
    void start_expired_rooms();
    void mark_disconnected(const std::string& player_id);
    void reconnect_player(const std::string& player_id);

    const std::vector<Room>& rooms() const;

  private:
    core::SecureRng& rng_;
    std::vector<Room> rooms_;
};

}  // namespace seka::server

#endif  // SEKA_SERVER_ROOM_MANAGER_H
