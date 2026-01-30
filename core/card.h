#ifndef SEKA_CORE_CARD_H
#define SEKA_CORE_CARD_H

#include <string>

namespace seka::core {

enum class Suit {
    Spades,
    Hearts,
    Diamonds,
    Clubs,
    None
};

enum class Rank {
    Ten = 10,
    Jack = 11,
    Queen = 12,
    King = 13,
    Ace = 14,
    Joker = 0
};

struct Card {
    Rank rank{};
    Suit suit{Suit::None};
    bool is_joker{false};

    Card() = default;
    Card(Rank card_rank, Suit card_suit, bool joker = false);

    std::string to_string() const;
    int value() const;
};

}  // namespace seka::core

#endif  // SEKA_CORE_CARD_H
