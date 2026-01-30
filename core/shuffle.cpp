#include "core/shuffle.h"

#include <cstddef>
#include <limits>

namespace seka::core {

namespace {

std::size_t random_index(std::size_t upper_bound, SecureRng& rng) {
    if (upper_bound == 0) {
        return 0;
    }
    std::uint32_t value = 0;
    std::uint32_t range = static_cast<std::uint32_t>(upper_bound + 1);
    std::uint32_t limit = std::numeric_limits<std::uint32_t>::max()
                          - (std::numeric_limits<std::uint32_t>::max() % range);
    do {
        rng.fill_bytes(reinterpret_cast<std::uint8_t*>(&value), sizeof(value));
    } while (value >= limit);
    return static_cast<std::size_t>(value % range);
}

}  // namespace

void shuffle_deck(std::vector<Card>& deck, SecureRng& rng) {
    if (deck.empty()) {
        return;
    }
    for (std::size_t i = deck.size() - 1; i > 0; --i) {
        std::size_t j = random_index(i, rng);
        std::swap(deck[i], deck[j]);
    }
}

}  // namespace seka::core
