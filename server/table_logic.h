#ifndef SEKA_SERVER_TABLE_LOGIC_H
#define SEKA_SERVER_TABLE_LOGIC_H

#include <string>

#include "core/secure_rng.h"
#include "server/models.h"

namespace seka::server {

void prepare_seed(Room& room, core::SecureRng& rng);
void deal_room(Room& room);
std::string room_winner_id(const Room& room);

}  // namespace seka::server

#endif  // SEKA_SERVER_TABLE_LOGIC_H
