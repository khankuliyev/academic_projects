#include <cassert>
#include <iostream>

#include "core/secure_rng.h"
#include "server/tournament_manager.h"

int main() {
    auto rng = seka::core::create_platform_rng();
    seka::server::TournamentManager manager(*rng);
    auto& tournament = manager.create_tournament();

    for (int i = 0; i < 12; ++i) {
        manager.join_tournament(tournament.id, "player_" + std::to_string(i), "P");
    }

    bool started = manager.start_tournament(tournament);
    assert(started);
    assert(tournament.state == seka::server::TournamentState::Running || tournament.state == seka::server::TournamentState::Finished);
    if (tournament.state == seka::server::TournamentState::Running) {
        assert(tournament.tables.size() == 2);
    }

    std::cout << "Tournament test passed.\n";
    return 0;
}
