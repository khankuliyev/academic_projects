#ifndef SEKA_UI_H
#define SEKA_UI_H

#include <string>

#include "seka/game.h"

namespace seka {

class UI {
  public:
    static int show_main_menu();
    static void show_rules();
    static int prompt_int(const std::string& prompt, int min_value, int max_value);
    static std::string prompt_action(const std::string& prompt, const std::string& valid);
    static void show_hand(const Player& player, bool reveal);
    static void show_message(const std::string& message);
};

}  // namespace seka

#endif  // SEKA_UI_H
