#ifndef SEKA_GAME_H
#define SEKA_GAME_H

#include <string>
#include <vector>

#include "seka/deck.h"
#include "seka/hand.h"

namespace seka {

struct Player {
    std::string name;
    bool is_bot;
    int bank;
    bool folded = false;
    int current_bet = 0;
    Hand hand;
};

struct GameOptions {
    int num_players = 2;
    int starting_bank = 100;
    int min_bet = 5;
};

class Game {
  public:
    explicit Game(const GameOptions& options);

    /**
     * Runs the game loop until one player remains.
     */
    void run();

  private:
    GameOptions options_;
    std::vector<Player> players_;
    Deck deck_;
    int pot_ = 0;
    int dealer_index_ = 0;

    void setup_players();
    void start_round();
    void betting_round();
    void showdown();
    void remove_broke_players();

    int active_player_count() const;
};

}  // namespace seka

#endif  // SEKA_GAME_H
