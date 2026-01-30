#include <cassert>
#include <cmath>
#include <iostream>
#include <unordered_map>

#include "core/deck.h"
#include "core/secure_rng.h"
#include "core/shuffle.h"

int main() {
    constexpr int kShuffles = 100000;
    seka::core::Deck reference;
    std::unordered_map<std::string, std::size_t> card_index;
    std::size_t index = 0;
    for (const auto& card : reference.cards()) {
        card_index[card.to_string()] = index++;
    }

    std::vector<std::vector<int>> counts(21, std::vector<int>(21, 0));
    auto rng = seka::core::create_platform_rng();

    for (int i = 0; i < kShuffles; ++i) {
        seka::core::Deck deck;
        auto& cards = deck.cards();
        seka::core::shuffle_deck(cards, *rng);
        assert(cards.size() == 21);

        std::unordered_map<std::string, int> seen;
        for (std::size_t pos = 0; pos < cards.size(); ++pos) {
            const auto& card = cards[pos];
            ++seen[card.to_string()];
            counts[card_index[card.to_string()]][pos] += 1;
        }
        for (const auto& item : seen) {
            assert(item.second == 1);
        }
    }

    double expected = static_cast<double>(kShuffles) / 21.0;
    double max_chi = 0.0;
    for (const auto& card_counts : counts) {
        double chi = 0.0;
        for (int count : card_counts) {
            double diff = count - expected;
            chi += (diff * diff) / expected;
        }
        if (chi > max_chi) {
            max_chi = chi;
        }
    }

    std::cout << "Max chi-square across cards: " << max_chi << "\n";
    assert(max_chi < 60.0);
    return 0;
}
