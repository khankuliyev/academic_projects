#include <cassert>
#include <iostream>

#include "core/deck.h"
#include "core/evaluator.h"
#include "core/hand.h"

using seka::core::Card;
using seka::core::Deck;
using seka::core::Hand;
using seka::core::HandCategory;
using seka::core::HandValue;
using seka::core::Rank;
using seka::core::Suit;

void test_deck() {
    Deck deck;
    assert(deck.cards().size() == 21);

    int joker_count = 0;
    for (const auto& card : deck.cards()) {
        if (card.is_joker) {
            ++joker_count;
        }
    }
    assert(joker_count == 1);
}

void test_joker_copy_best() {
    Hand hand({Card(Rank::Ten, Suit::Spades), Card(Rank::Ace, Suit::Hearts), Card(Rank::Joker, Suit::None, true)});
    HandValue value = seka::core::evaluate_hand(hand);
    assert(value.category == HandCategory::Pair);
    assert(value.kickers[0] == static_cast<int>(Rank::Ace));
}

void test_joker_maximization() {
    Hand hand({Card(Rank::Ten, Suit::Spades), Card(Rank::King, Suit::Hearts), Card(Rank::Joker, Suit::None, true)});
    HandValue value = seka::core::evaluate_hand(hand);
    assert(value.category == HandCategory::Pair);
    assert(value.kickers[0] == static_cast<int>(Rank::King));
}

void test_hand_comparison() {
    Hand three_kind({Card(Rank::Queen, Suit::Spades), Card(Rank::Queen, Suit::Hearts), Card(Rank::Queen, Suit::Diamonds)});
    Hand straight({Card(Rank::Ten, Suit::Spades), Card(Rank::Jack, Suit::Hearts), Card(Rank::Queen, Suit::Clubs)});

    HandValue value_three = seka::core::evaluate_hand(three_kind);
    HandValue value_straight = seka::core::evaluate_hand(straight);

    assert(value_three > value_straight);
}

int main() {
    test_deck();
    test_joker_copy_best();
    test_joker_maximization();
    test_hand_comparison();

    std::cout << "All tests passed.\n";
    return 0;
}
