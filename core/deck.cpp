#include "core/deck.h"

namespace seka::core {

Deck::Deck() {
    cards_.reserve(21);
    const Suit suits[] = {Suit::Spades, Suit::Hearts, Suit::Diamonds, Suit::Clubs};
    const Rank ranks[] = {Rank::Ten, Rank::Jack, Rank::Queen, Rank::King, Rank::Ace};

    for (Suit suit : suits) {
        for (Rank rank : ranks) {
            cards_.emplace_back(rank, suit, false);
        }
    }

    cards_.emplace_back(Rank::Joker, Suit::None, true);
}

const std::vector<Card>& Deck::cards() const {
    return cards_;
}

std::vector<Card>& Deck::cards() {
    return cards_;
}

}  // namespace seka::core
