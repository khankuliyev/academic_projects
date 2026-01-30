#ifndef SEKA_DECK_H
#define SEKA_DECK_H

#include <vector>

#include "seka/card.h"

namespace seka {

/**
 * Represents a 21-card Seka deck with a single joker.
 */
class Deck {
  public:
    Deck();

    /**
     * Shuffles the deck.
     */
    void shuffle();

    /**
     * Draws a card from the top of the deck.
     */
    Card draw();

    /**
     * Returns remaining number of cards.
     */
    std::size_t size() const;

  private:
    std::vector<Card> cards_;
};

}  // namespace seka

#endif  // SEKA_DECK_H
