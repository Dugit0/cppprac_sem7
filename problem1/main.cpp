#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <print>
#include <vector>
#include <string>

#include "shared_pointer.cpp"


class Player {
public:
    Player() {};
    virtual ~Player() {};
};

class Civilian : public Player {
public:
};

class Mafia : public Player {
public:
};

class Maniac : public Player {
public:
};


class Game {
public:
    std::vector<Player*> players;
    unsigned int players_num;
    unsigned int mafia_modifier;
    std::vector<std::string> civilian_roles {"commissar", "doctor", "journalist", "samurai"};
    std::vector<std::string> mafia_roles {"bull"};
// "civilian", "mafia", "maniac"

    Game(unsigned int players_num_, unsigned int mafia_modifier_ = 3) : 
        players_num(players_num_),
        mafia_modifier(mafia_modifier_)
    {}

    std::vector<std::string> get_random_roles() {
        unsigned int mafia_num = players_num / mafia_modifier;
        auto buf{civilian_roles};
        // for (auto a : buf) std::cout << a << " "; std::cout << std::endl;
        std::print("{}", buf);
        return buf;
    }

    decltype(players) init_players(std::vector<std::string> roles);

};


int main(void) {
    std::srand(42);
    auto game = Game{10};
    game.get_random_roles();
    return 0;
}
