#include "seka/hand.h"

namespace seka {

Hand::Hand(const std::vector<Card>& cards) : cards_(cards) {}

void Hand::add_card(const Card& card) {
    cards_.push_back(card);
}

const std::vector<Card>& Hand::cards() const {
    return cards_;
}

}  // namespace seka
