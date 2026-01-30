#ifndef SEKA_SERVER_PROTOCOL_H
#define SEKA_SERVER_PROTOCOL_H

#include <string>

#include <boost/json.hpp>

#include "server/models.h"

namespace seka::server {

boost::json::object make_room_state(const Room& room);
boost::json::object make_tournament_state(const Tournament& tournament);

}  // namespace seka::server

#endif  // SEKA_SERVER_PROTOCOL_H
