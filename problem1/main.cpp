#define DEBUG

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <vector>
#include <string>
#include <ranges>

#include "shared_pointer.cpp"

namespace view = std::ranges::views;

#ifdef DEBUG
#include "formatter.cpp"

template<typename T>
void print(T value) {
    std::cout << Format(value) << std::endl;
}
#endif




class Player {
public:
    Player() {
        alive = true;
    };
    virtual ~Player() {};
    
    bool alive;
    std::string team;
};

class Civilian : public Player {
public:
    Civilian() {
        team = "civilian";
    }
    virtual ~Civilian() {};
};

class Mafia : public Player {
public:
    Mafia() {
        team = "mafia";
    }
    virtual ~Mafia() {};
};

class Maniac : public Player {
public:
    Maniac() {
        team = "maniac";
    }
    virtual ~Maniac() {};
};


class Game {
public:
    std::vector<Shared_pointer<Player>> players {};
    unsigned int players_num;
    unsigned int mafia_modifier;
    std::vector<std::string> civilian_roles {"commissar", "doctor", "journalist", "samurai"};
    std::vector<std::string> mafia_roles {"bull"};
// "civilian", "mafia", "maniac"

    explicit Game(unsigned int players_num_, unsigned int mafia_modifier_ = 3) : 
        players_num(players_num_),
        mafia_modifier(mafia_modifier_)
    {}
    
    void add_random_roles(
            std::vector<std::string> roles,
            size_t limit,
            std::string default_role,
            std::vector<std::string>& result) {
        std::mt19937 g(rand());
        size_t i = 0;
        std::shuffle(roles.begin(), roles.end(), g);
        while (i < limit) {
            if (i < roles.size()) {
                result.push_back(roles[i]);
            } else {
                result.push_back(default_role);
            }
            i++;
        }
    }

    std::vector<std::string> get_random_roles() {
        unsigned int mafia_num = players_num / mafia_modifier;
        std::vector<std::string> random_roles;
        
        // for (auto a : buf) std::cout << a << " "; std::cout << std::endl;
        add_random_roles(mafia_roles, mafia_num, "mafia", random_roles);
        random_roles.push_back("maniac");
        add_random_roles(civilian_roles, players_num - mafia_num - 1, "civilian", random_roles);
        std::mt19937 g(rand());
        std::shuffle(random_roles.begin(), random_roles.end(), g);
        return random_roles;
    }

    void init_players(std::vector<std::string> roles) {
        players.clear();
        for (auto role : roles) {
            if (role == "civilian") {
                players.push_back(Shared_pointer<Player>(new Civilian{}));
            } else if (role == "mafia") {
                players.push_back(Shared_pointer<Player>(new Mafia{}));
            } else if (role == "maniac") {
                players.push_back(Shared_pointer<Player>(new Maniac{}));
            } else if (role == "bull") {
            } else if (role == "commissar") {
            } else if (role == "doctor") {
            } else if (role == "journalist") {
            } else if (role == "samurai") {
            }
        }
    }

    std::string game_status() {
        auto alives = players | view::filter([](auto p) { return p->alive; });
        if (alives.empty()) {
            // Everyone die
            return "draw";
        } else {
            auto mafia = alives | view::filter([](auto p) { return p->team == "mafia"; });
            if (mafia.empty()) {
                // Mafia die
                if ((alives | view::filter([](auto p) { return p->team == "maniac"; })).empty()) {
                    // Maniac die
                    return "civilian";
                } else {
                    // Maniac is alive
                    if (std::ranges::distance(alives) >= 3) {
                        return "continue";
                    } else {
                        return "maniac";
                    }
                }
            } else {
                // Mafia is alive
                if ((alives | view::filter([](auto p) { return p->team == "maniac"; })).empty()) {
                    // Maniac die
                    auto alives_num = std::ranges::distance(alives);
                    auto mafia_num = std::ranges::distance(mafia);
                    if (alives_num <= mafia_num * 2) {
                        return "mafia";
                    } else {
                        return "continue";
                    }
                } else {
                    // Maniac is alive
                    return "continue";
                }
            }
        }
    }
};


void debug(std::vector<std::string> v) {
    auto game = Game{10};
    game.init_players(v);
    print(game.game_status());
}

int main(void) {
    std::srand(42);
    debug({}); // draw
    debug({"civilian"}); // civ
    debug({"civilian", "civilian"}); // civ
    debug({"civilian", "maniac"}); // maniac
    debug({"civilian", "civilian", "maniac"}); // cont
    debug({"mafia", "civilian", "maniac"});
    debug({"mafia", "civilian", "civilian"});
    debug({"mafia", "mafia", "civilian"});
    return 0;
}
