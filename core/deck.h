#ifndef SEKA_CORE_DECK_H
#define SEKA_CORE_DECK_H

#include <vector>

#include "core/card.h"

namespace seka::core {

class Deck {
  public:
    Deck();

    const std::vector<Card>& cards() const;
    std::vector<Card>& cards();

  private:
    std::vector<Card> cards_;
};

}  // namespace seka::core

#endif  // SEKA_CORE_DECK_H
