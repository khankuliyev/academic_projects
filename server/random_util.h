#ifndef SEKA_SERVER_RANDOM_UTIL_H
#define SEKA_SERVER_RANDOM_UTIL_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "core/secure_rng.h"

namespace seka::server {

std::string random_hex(core::SecureRng& rng, std::size_t bytes);
std::string random_base32(core::SecureRng& rng, std::size_t length);
std::vector<std::uint8_t> random_bytes(core::SecureRng& rng, std::size_t bytes);
std::string hex_from_bytes(const std::vector<std::uint8_t>& data);

}  // namespace seka::server

#endif  // SEKA_SERVER_RANDOM_UTIL_H
