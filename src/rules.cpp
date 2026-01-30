#include "seka/rules.h"

#include <algorithm>
#include <stdexcept>

namespace seka {

namespace {

bool is_flush(const std::vector<Card>& cards) {
    if (cards.empty()) {
        return false;
    }
    Suit suit = cards.front().suit;
    if (suit == Suit::None) {
        return false;
    }
    for (const auto& card : cards) {
        if (card.suit != suit || card.suit == Suit::None) {
            return false;
        }
    }
    return true;
}

bool is_straight(const std::vector<int>& values) {
    if (values.size() != 3) {
        return false;
    }
    return values[0] + 1 == values[1] && values[1] + 1 == values[2];
}

HandValue evaluate_without_joker(const std::vector<Card>& cards) {
    if (cards.size() != 3) {
        throw std::runtime_error("Seka hand must contain exactly 3 cards.");
    }

    std::vector<int> values;
    values.reserve(cards.size());
    for (const auto& card : cards) {
        values.push_back(card.value());
    }
    std::sort(values.begin(), values.end());

    bool flush = is_flush(cards);
    bool straight = is_straight(values);

    bool three_kind = values[0] == values[1] && values[1] == values[2];
    bool pair = values[0] == values[1] || values[1] == values[2];

    if (three_kind) {
        return {HandCategory::ThreeOfAKind, 300 + values[2], {values[2]}};
    }
    if (straight && flush) {
        return {HandCategory::StraightFlush, 250 + values[2], {values[2]}};
    }
    if (flush) {
        int sum = values[0] + values[1] + values[2];
        return {HandCategory::Flush, 200 + sum, {values[2], values[1], values[0]}};
    }
    if (straight) {
        return {HandCategory::Straight, 150 + values[2], {values[2]}};
    }
    if (pair) {
        int pair_rank = values[1];
        int kicker = values[0] == values[1] ? values[2] : values[0];
        return {HandCategory::Pair, 100 + pair_rank * 10 + kicker, {pair_rank, kicker}};
    }

    return {HandCategory::HighCard, values[2], {values[2], values[1], values[0]}};
}

}  // namespace

bool HandValue::operator<(const HandValue& other) const {
    if (category != other.category) {
        return category < other.category;
    }
    if (score != other.score) {
        return score < other.score;
    }
    return kickers < other.kickers;
}

bool HandValue::operator>(const HandValue& other) const {
    return other < *this;
}

bool HandValue::operator==(const HandValue& other) const {
    return category == other.category && score == other.score && kickers == other.kickers;
}

HandValue evaluate_hand(const Hand& hand) {
    const auto& cards = hand.cards();
    if (cards.size() != 3) {
        throw std::runtime_error("Seka hand must contain exactly 3 cards.");
    }

    std::vector<Card> non_jokers;
    non_jokers.reserve(cards.size());
    int joker_count = 0;

    for (const auto& card : cards) {
        if (card.is_joker) {
            ++joker_count;
        } else {
            non_jokers.push_back(card);
        }
    }

    if (joker_count == 0) {
        return evaluate_without_joker(cards);
    }

    if (joker_count > 1) {
        throw std::runtime_error("Only one joker is supported in the deck.");
    }

    if (non_jokers.size() != 2) {
        throw std::runtime_error("Invalid joker hand structure.");
    }

    std::vector<HandValue> candidates;
    candidates.reserve(2);

    for (const auto& copy_source : non_jokers) {
        std::vector<Card> adjusted = non_jokers;
        adjusted.emplace_back(copy_source.rank, Suit::None, false);
        candidates.push_back(evaluate_without_joker(adjusted));
    }

    return *std::max_element(candidates.begin(), candidates.end());
}

}  // namespace seka
