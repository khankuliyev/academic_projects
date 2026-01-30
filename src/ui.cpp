#include "seka/ui.h"

#include <iostream>
#include <limits>

namespace seka {

int UI::show_main_menu() {
    std::cout << "\n=== SEKA ===\n";
    std::cout << "1. Новая игра\n";
    std::cout << "2. Правила\n";
    std::cout << "3. Выход\n";
    return prompt_int("Выберите пункт", 1, 3);
}

void UI::show_rules() {
    std::cout << "\nПравила (вариант):\n";
    std::cout << "- 3 карты на руку, 21-карточная колода (10-A + джокер).\n";
    std::cout << "- Минимальная ставка выступает как анте.\n";
    std::cout << "- Одна торговля: игроки могут уравнять, поднять на мин. ставку или спасовать.\n";
    std::cout << "- Джокер копирует лучшую карту по очкам, не дает масть для флеша.\n";
    std::cout << "- Комбинации (по убыванию): три одинаковых, стрит-флеш, флеш, стрит, пара, старшая карта.\n";
}

int UI::prompt_int(const std::string& prompt, int min_value, int max_value) {
    int value = 0;
    while (true) {
        std::cout << prompt << " (" << min_value << "-" << max_value << "): ";
        if (std::cin >> value && value >= min_value && value <= max_value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
        std::cout << "Некорректный ввод.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string UI::prompt_action(const std::string& prompt, const std::string& valid) {
    std::string input;
    while (true) {
        std::cout << prompt << " [" << valid << "]: ";
        if (std::getline(std::cin, input)) {
            if (!input.empty()) {
                char c = static_cast<char>(std::tolower(input[0]));
                if (valid.find(c) != std::string::npos) {
                    return std::string(1, c);
                }
            }
        }
        std::cout << "Некорректный ввод.\n";
    }
}

void UI::show_hand(const Player& player, bool reveal) {
    std::cout << player.name << " карты: ";
    if (!reveal) {
        std::cout << "[скрыты]";
    } else {
        for (const auto& card : player.hand.cards()) {
            std::cout << card.to_string() << " ";
        }
    }
    std::cout << "\n";
}

void UI::show_message(const std::string& message) {
    std::cout << message << "\n";
}

}  // namespace seka
