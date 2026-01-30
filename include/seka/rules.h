#ifndef SEKA_RULES_H
#define SEKA_RULES_H

#include <vector>

#include "seka/card.h"
#include "seka/hand.h"

namespace seka {

enum class HandCategory {
    HighCard = 0,
    Pair = 1,
    Straight = 2,
    Flush = 3,
    StraightFlush = 4,
    ThreeOfAKind = 5
};

/**
 * Hand evaluation result used for comparison.
 */
struct HandValue {
    HandCategory category;
    int score;
    std::vector<int> kickers;

    bool operator<(const HandValue& other) const;
    bool operator>(const HandValue& other) const;
    bool operator==(const HandValue& other) const;
};

/**
 * Evaluates a hand according to the chosen Seka rules variant.
 */
HandValue evaluate_hand(const Hand& hand);

}  // namespace seka

#endif  // SEKA_RULES_H
