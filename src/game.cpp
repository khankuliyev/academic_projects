#include "seka/game.h"

#include <algorithm>
#include <iostream>
#include <random>

#include "seka/rules.h"
#include "seka/ui.h"

namespace seka {

Game::Game(const GameOptions& options) : options_(options) {
    setup_players();
}

void Game::setup_players() {
    players_.clear();
    players_.reserve(options_.num_players);
    players_.push_back({"Вы", false, options_.starting_bank});
    for (int i = 1; i < options_.num_players; ++i) {
        players_.push_back({"Bot " + std::to_string(i), true, options_.starting_bank});
    }
}

void Game::run() {
    while (active_player_count() > 1) {
        start_round();
        betting_round();
        showdown();
        remove_broke_players();
        dealer_index_ = (dealer_index_ + 1) % static_cast<int>(players_.size());
    }

    if (!players_.empty()) {
        UI::show_message("Победитель: " + players_.front().name);
    }
}

void Game::start_round() {
    deck_ = Deck();
    deck_.shuffle();
    pot_ = 0;

    for (auto& player : players_) {
        player.folded = false;
        player.current_bet = 0;
        player.hand = Hand();
    }

    for (auto& player : players_) {
        if (player.bank < options_.min_bet) {
            player.folded = true;
            continue;
        }
        player.bank -= options_.min_bet;
        player.current_bet = options_.min_bet;
        pot_ += options_.min_bet;
    }

    for (int i = 0; i < 3; ++i) {
        for (auto& player : players_) {
            if (!player.folded) {
                player.hand.add_card(deck_.draw());
            }
        }
    }

    UI::show_message("\n--- Новый раунд ---");
    UI::show_message("Банк: " + std::to_string(pot_));
}

void Game::betting_round() {
    int highest_bet = options_.min_bet;
    std::random_device device;
    std::mt19937 rng(device());
    std::uniform_int_distribution<int> coin(0, 1);

    for (auto& player : players_) {
        if (player.folded) {
            continue;
        }

        if (!player.is_bot) {
            UI::show_hand(player, true);
            std::string action = UI::prompt_action("Действие: (c)all/(r)aise/(f)old", "crf");
            if (action == "f") {
                player.folded = true;
                continue;
            }
            if (action == "r") {
                highest_bet += options_.min_bet;
            }
        } else {
            HandValue value = evaluate_hand(player.hand);
            if (value.category >= HandCategory::Straight) {
                highest_bet += options_.min_bet;
                UI::show_message(player.name + " повышает ставку.");
            } else if (value.category >= HandCategory::Pair || coin(rng) == 1) {
                UI::show_message(player.name + " уравнивает.");
            } else {
                UI::show_message(player.name + " пасует.");
                player.folded = true;
                continue;
            }
        }

        int need_to_call = highest_bet - player.current_bet;
        if (need_to_call > player.bank) {
            player.folded = true;
            UI::show_message(player.name + " не хватает фишек и пасует.");
            continue;
        }

        player.bank -= need_to_call;
        player.current_bet += need_to_call;
        pot_ += need_to_call;
    }

    UI::show_message("Банк после торговли: " + std::to_string(pot_));
}

void Game::showdown() {
    std::vector<Player*> contenders;
    for (auto& player : players_) {
        if (!player.folded) {
            contenders.push_back(&player);
        }
    }

    if (contenders.empty()) {
        UI::show_message("Все пасовали. Банк переносится.");
        return;
    }

    UI::show_message("\n--- Вскрытие ---");

    HandValue best_value{HandCategory::HighCard, 0, {}};
    for (const auto* player : contenders) {
        UI::show_hand(*player, true);
        HandValue value = evaluate_hand(player->hand);
        if (value > best_value) {
            best_value = value;
        }
    }

    std::vector<Player*> winners;
    for (auto* player : contenders) {
        if (evaluate_hand(player->hand) == best_value) {
            winners.push_back(player);
        }
    }

    int share = pot_ / static_cast<int>(winners.size());
    for (auto* winner : winners) {
        winner->bank += share;
    }

    if (winners.size() == 1) {
        UI::show_message("Победитель раунда: " + winners.front()->name + ", выигрыш " + std::to_string(share));
    } else {
        UI::show_message("Ничья. Банк делится между " + std::to_string(winners.size()) + " игроками.");
    }

    pot_ = 0;
}

void Game::remove_broke_players() {
    players_.erase(std::remove_if(players_.begin(), players_.end(), [](const Player& player) {
        return player.bank <= 0;
    }), players_.end());
}

int Game::active_player_count() const {
    return static_cast<int>(players_.size());
}

}  // namespace seka
