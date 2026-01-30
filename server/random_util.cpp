#include "server/random_util.h"

#include <array>
#include <iomanip>
#include <sstream>

namespace seka::server {

std::vector<std::uint8_t> random_bytes(core::SecureRng& rng, std::size_t bytes) {
    std::vector<std::uint8_t> data(bytes);
    if (!data.empty()) {
        rng.fill_bytes(data.data(), data.size());
    }
    return data;
}

std::string random_hex(core::SecureRng& rng, std::size_t bytes) {
    auto data = random_bytes(rng, bytes);
    return hex_from_bytes(data);
}

std::string random_base32(core::SecureRng& rng, std::size_t length) {
    static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    std::string result;
    result.reserve(length);
    auto data = random_bytes(rng, length);
    for (auto byte : data) {
        result.push_back(alphabet[byte % 32]);
    }
    return result;
}

std::string hex_from_bytes(const std::vector<std::uint8_t>& data) {
    std::ostringstream out;
    for (auto byte : data) {
        out << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return out.str();
}

}  // namespace seka::server
