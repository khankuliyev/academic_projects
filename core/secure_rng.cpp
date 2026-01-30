#include "core/secure_rng.h"

#include <array>
#include <stdexcept>

#if defined(_WIN32)
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#elif defined(__APPLE__)
#include <stdlib.h>
#else
#include <fstream>
#endif

namespace seka::core {

namespace {

#if defined(_WIN32)
class WindowsSecureRng final : public SecureRng {
  public:
    void fill_bytes(std::uint8_t* data, std::size_t size) override {
        if (BCryptGenRandom(nullptr, data, static_cast<ULONG>(size), BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
            throw std::runtime_error("BCryptGenRandom failed.");
        }
    }
};
#elif defined(__APPLE__)
class AppleSecureRng final : public SecureRng {
  public:
    void fill_bytes(std::uint8_t* data, std::size_t size) override {
        arc4random_buf(data, size);
    }
};
#else
class LinuxSecureRng final : public SecureRng {
  public:
    void fill_bytes(std::uint8_t* data, std::size_t size) override {
        std::ifstream stream("/dev/urandom", std::ios::in | std::ios::binary);
        if (!stream) {
            throw std::runtime_error("Unable to open /dev/urandom.");
        }
        stream.read(reinterpret_cast<char*>(data), static_cast<std::streamsize>(size));
        if (!stream) {
            throw std::runtime_error("Failed to read from /dev/urandom.");
        }
    }
};
#endif

}  // namespace

std::unique_ptr<SecureRng> create_platform_rng() {
#if defined(_WIN32)
    return std::make_unique<WindowsSecureRng>();
#elif defined(__APPLE__)
    return std::make_unique<AppleSecureRng>();
#else
    return std::make_unique<LinuxSecureRng>();
#endif
}

}  // namespace seka::core
