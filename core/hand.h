#ifndef SEKA_CORE_HAND_H
#define SEKA_CORE_HAND_H

#include <vector>

#include "core/card.h"

namespace seka::core {

class Hand {
  public:
    Hand() = default;
    explicit Hand(std::vector<Card> cards);

    void add_card(const Card& card);
    const std::vector<Card>& cards() const;

  private:
    std::vector<Card> cards_;
};

}  // namespace seka::core

#endif  // SEKA_CORE_HAND_H
