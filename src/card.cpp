#include "seka/card.h"

namespace seka {

Card::Card(Rank card_rank, Suit card_suit, bool joker)
    : rank(card_rank), suit(card_suit), is_joker(joker) {}

int Card::value() const {
    if (is_joker) {
        return 0;
    }
    return static_cast<int>(rank);
}

std::string Card::to_string() const {
    if (is_joker) {
        return "Joker";
    }

    std::string rank_str;
    switch (rank) {
        case Rank::Ten:
            rank_str = "10";
            break;
        case Rank::Jack:
            rank_str = "J";
            break;
        case Rank::Queen:
            rank_str = "Q";
            break;
        case Rank::King:
            rank_str = "K";
            break;
        case Rank::Ace:
            rank_str = "A";
            break;
        default:
            rank_str = "?";
            break;
    }

    std::string suit_str;
    switch (suit) {
        case Suit::Spades:
            suit_str = "♠";
            break;
        case Suit::Hearts:
            suit_str = "♥";
            break;
        case Suit::Diamonds:
            suit_str = "♦";
            break;
        case Suit::Clubs:
            suit_str = "♣";
            break;
        default:
            suit_str = "";
            break;
    }

    return rank_str + suit_str;
}

}  // namespace seka
