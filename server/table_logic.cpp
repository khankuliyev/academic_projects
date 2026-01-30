#include "server/table_logic.h"

#include <algorithm>
#include <cstring>
#include <random>

#include "core/deck.h"
#include "core/evaluator.h"
#include "core/sha256.h"
#include "core/shuffle.h"
#include "server/random_util.h"

namespace seka::server {

class SeededRng final : public core::SecureRng {
  public:
    explicit SeededRng(const std::vector<std::uint8_t>& seed_bytes) {
        std::seed_seq seq(seed_bytes.begin(), seed_bytes.end());
        engine_.seed(seq);
    }

    void fill_bytes(std::uint8_t* data, std::size_t size) override {
        std::uniform_int_distribution<std::uint32_t> dist(0, 0xffffffff);
        std::size_t offset = 0;
        while (offset < size) {
            std::uint32_t value = dist(engine_);
            std::size_t chunk = std::min<std::size_t>(sizeof(value), size - offset);
            std::memcpy(data + offset, &value, chunk);
            offset += chunk;
        }
    }

  private:
    std::mt19937_64 engine_;
};

void prepare_seed(Room& room, core::SecureRng& rng) {
    room.seed_bytes = random_bytes(rng, 32);
    room.seed_hash = core::sha256_hex(room.seed_bytes);
    room.revealed_seed.clear();
}

void deal_room(Room& room) {
    core::Deck deck;
    SeededRng seeded_rng(room.seed_bytes);
    core::shuffle_deck(deck.cards(), seeded_rng);
    std::size_t index = 0;
    for (auto& player : room.players) {
        if (player.status == PlayerStatus::Eliminated) {
            continue;
        }
        std::vector<core::Card> hand_cards;
        hand_cards.reserve(3);
        for (int i = 0; i < 3; ++i) {
            hand_cards.push_back(deck.cards()[index++]);
        }
        player.hand = core::Hand(std::move(hand_cards));
    }

    room.revealed_seed = hex_from_bytes(room.seed_bytes);
}

std::string room_winner_id(const Room& room) {
    core::HandValue best{core::HandCategory::HighCard, 0, {}};
    std::string winner;
    for (const auto& player : room.players) {
        if (player.status == PlayerStatus::Eliminated) {
            continue;
        }
        auto value = core::evaluate_hand(player.hand);
        if (winner.empty() || value > best) {
            best = value;
            winner = player.id;
        }
    }
    return winner;
}

}  // namespace seka::server
