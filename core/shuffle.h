#ifndef SEKA_CORE_SHUFFLE_H
#define SEKA_CORE_SHUFFLE_H

#include <vector>

#include "core/card.h"
#include "core/secure_rng.h"

namespace seka::core {

void shuffle_deck(std::vector<Card>& deck, SecureRng& rng);

}  // namespace seka::core

#endif  // SEKA_CORE_SHUFFLE_H
