#include "seka/game.h"
#include "seka/ui.h"

using seka::Game;
using seka::GameOptions;
using seka::UI;

int main() {
    while (true) {
        int choice = UI::show_main_menu();
        if (choice == 1) {
            GameOptions options;
            options.num_players = UI::prompt_int("Количество игроков", 2, 7);
            options.starting_bank = UI::prompt_int("Стартовый банк", 20, 1000);
            options.min_bet = UI::prompt_int("Минимальная ставка", 1, options.starting_bank);

            Game game(options);
            game.run();
        } else if (choice == 2) {
            UI::show_rules();
        } else {
            break;
        }
    }

    return 0;
}
