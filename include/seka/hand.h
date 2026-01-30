#ifndef SEKA_HAND_H
#define SEKA_HAND_H

#include <vector>

#include "seka/card.h"

namespace seka {

/**
 * Represents a player's hand (three cards in Seka).
 */
class Hand {
  public:
    Hand() = default;
    explicit Hand(const std::vector<Card>& cards);

    void add_card(const Card& card);

    const std::vector<Card>& cards() const;

  private:
    std::vector<Card> cards_;
};

}  // namespace seka

#endif  // SEKA_HAND_H
