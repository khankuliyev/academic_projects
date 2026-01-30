#ifndef SEKA_CORE_SHA256_H
#define SEKA_CORE_SHA256_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace seka::core {

class Sha256 {
  public:
    Sha256();
    void update(const std::uint8_t* data, std::size_t len);
    std::array<std::uint8_t, 32> digest();
    static std::string hex_digest(const std::vector<std::uint8_t>& data);

  private:
    void transform(const std::uint8_t* chunk);

    std::uint64_t bit_len_;
    std::array<std::uint8_t, 64> buffer_;
    std::size_t buffer_len_;
    std::array<std::uint32_t, 8> state_;
};

std::string sha256_hex(const std::vector<std::uint8_t>& data);

}  // namespace seka::core

#endif  // SEKA_CORE_SHA256_H
