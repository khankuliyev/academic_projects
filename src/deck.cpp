#include "seka/deck.h"

#include <algorithm>
#include <random>
#include <stdexcept>

namespace seka {

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

void Deck::shuffle() {
    std::random_device device;
    std::mt19937 generator(device());
    std::shuffle(cards_.begin(), cards_.end(), generator);
}

Card Deck::draw() {
    if (cards_.empty()) {
        throw std::runtime_error("Deck is empty.");
    }
    Card card = cards_.back();
    cards_.pop_back();
    return card;
}

std::size_t Deck::size() const {
    return cards_.size();
}

}  // namespace seka
