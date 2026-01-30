#ifndef SEKA_CORE_EVALUATOR_H
#define SEKA_CORE_EVALUATOR_H

#include <vector>

#include "core/hand.h"

namespace seka::core {

enum class HandCategory {
    HighCard = 0,
    Pair = 1,
    Straight = 2,
    Flush = 3,
    StraightFlush = 4,
    ThreeOfAKind = 5
};

struct HandValue {
    HandCategory category;
    int score;
    std::vector<int> kickers;

    bool operator<(const HandValue& other) const;
    bool operator>(const HandValue& other) const;
    bool operator==(const HandValue& other) const;
};

HandValue evaluate_hand(const Hand& hand);

}  // namespace seka::core

#endif  // SEKA_CORE_EVALUATOR_H
