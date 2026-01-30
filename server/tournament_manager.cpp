#include "server/tournament_manager.h"

#include <algorithm>

#include "server/random_util.h"
#include "server/table_logic.h"

namespace seka::server {

TournamentManager::TournamentManager(core::SecureRng& rng) : rng_(rng) {}

Tournament& TournamentManager::create_tournament() {
    Tournament tournament;
    tournament.id = random_hex(rng_, 8);
    tournaments_.push_back(tournament);
    return tournaments_.back();
}

bool TournamentManager::join_tournament(const std::string& tournament_id,
                                        const std::string& player_id,
                                        const std::string& name) {
    auto tournament_opt = find_tournament(tournament_id);
    if (!tournament_opt) {
        return false;
    }
    auto& tournament = tournament_opt->get();
    if (tournament.state != TournamentState::Registering || tournament.participants.size() >= 300) {
        return false;
    }
    tournament.participants.push_back({player_id, name, PlayerStatus::Connected});
    return true;
}

std::optional<std::reference_wrapper<Tournament>> TournamentManager::find_tournament(const std::string& tournament_id) {
    for (auto& tournament : tournaments_) {
        if (tournament.id == tournament_id) {
            return tournament;
        }
    }
    return std::nullopt;
}

bool TournamentManager::start_tournament(Tournament& tournament) {
    if (tournament.state != TournamentState::Registering || tournament.participants.size() < 2) {
        return false;
    }
    tournament.state = TournamentState::Running;
    advance_round(tournament);
    return true;
}

void TournamentManager::advance_round(Tournament& tournament) {
    if (tournament.state != TournamentState::Running) {
        return;
    }

    std::vector<PlayerState> remaining;
    for (const auto& player : tournament.participants) {
        if (player.status != PlayerStatus::Eliminated) {
            remaining.push_back(player);
        }
    }

    if (remaining.size() <= 1) {
        tournament.state = TournamentState::Finished;
        if (!remaining.empty()) {
            tournament.winner_id = remaining.front().id;
        }
        return;
    }

    tournament.tables.clear();
    std::size_t index = 0;
    while (index < remaining.size()) {
        std::size_t table_size = std::min<std::size_t>(7, remaining.size() - index);
        if (remaining.size() - index >= 8 && remaining.size() - index <= 13) {
            table_size = 7;
        }
        Room table;
        table.id = random_hex(rng_, 8);
        table.state = RoomState::Playing;
        table.players.assign(remaining.begin() + static_cast<long>(index),
                             remaining.begin() + static_cast<long>(index + table_size));
        prepare_seed(table, rng_);
        deal_room(table);
        tournament.tables.push_back(table);
        index += table_size;
    }

    std::vector<PlayerState> winners;
    for (const auto& table : tournament.tables) {
        std::string winner_id = room_winner_id(table);
        for (const auto& player : table.players) {
            if (player.id == winner_id) {
                winners.push_back(player);
            }
        }
    }

    tournament.participants = winners;
    if (tournament.participants.size() == 1) {
        tournament.state = TournamentState::Finished;
        tournament.winner_id = tournament.participants.front().id;
    }
}

}  // namespace seka::server
