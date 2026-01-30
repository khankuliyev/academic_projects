#ifndef SEKA_SERVER_TOURNAMENT_MANAGER_H
#define SEKA_SERVER_TOURNAMENT_MANAGER_H

#include <optional>
#include <vector>

#include "core/secure_rng.h"
#include "server/models.h"

namespace seka::server {

class TournamentManager {
  public:
    explicit TournamentManager(core::SecureRng& rng);

    Tournament& create_tournament();
    bool join_tournament(const std::string& tournament_id, const std::string& player_id, const std::string& name);
    std::optional<std::reference_wrapper<Tournament>> find_tournament(const std::string& tournament_id);
    bool start_tournament(Tournament& tournament);
    void advance_round(Tournament& tournament);

  private:
    core::SecureRng& rng_;
    std::vector<Tournament> tournaments_;
};

}  // namespace seka::server

#endif  // SEKA_SERVER_TOURNAMENT_MANAGER_H
