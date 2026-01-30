#include "core/hand.h"

namespace seka::core {

Hand::Hand(std::vector<Card> cards) : cards_(std::move(cards)) {}

void Hand::add_card(const Card& card) {
    cards_.push_back(card);
}

const std::vector<Card>& Hand::cards() const {
    return cards_;
}

}  // namespace seka::core
