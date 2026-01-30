#ifndef SEKA_CORE_SECURE_RNG_H
#define SEKA_CORE_SECURE_RNG_H

#include <cstddef>
#include <cstdint>
#include <memory>

namespace seka::core {

class SecureRng {
  public:
    virtual ~SecureRng() = default;
    virtual void fill_bytes(std::uint8_t* data, std::size_t size) = 0;
};

std::unique_ptr<SecureRng> create_platform_rng();

}  // namespace seka::core

#endif  // SEKA_CORE_SECURE_RNG_H
