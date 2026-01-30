#ifndef SEKA_CARD_H
#define SEKA_CARD_H

#include <string>

namespace seka {

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

/**
 * Represents a playing card in the Seka deck.
 */
struct Card {
    Rank rank;
    Suit suit;
    bool is_joker;

    Card(Rank card_rank, Suit card_suit, bool joker = false);

    /**
     * Returns a short label like "A♠" or "Joker".
     */
    std::string to_string() const;

    /**
     * Returns numerical value for comparison in rules.
     */
    int value() const;
};

}  // namespace seka

#endif  // SEKA_CARD_H
