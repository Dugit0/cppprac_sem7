#define DEBUG_MODE

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <ranges>

#include "shared_pointer.cpp"
#include "logger.cpp"



namespace view = std::ranges::views;

#ifdef DEBUG_MODE
#include "formatter.cpp"

template<typename T>
void print(T value) {
    std::cout << Format(value) << std::endl;
}
#endif


template<typename T>
void simple_shuffle(T& container) {
    std::mt19937 g(rand());
    std::shuffle(container.begin(), container.end(), g);
}


class Player {
public:
    Player(size_t id_p) : id(id_p) {
        alive = true;
        is_real_player = false;
    };
    virtual ~Player() {};
    // virtual size_t vote(std::vector<size_t> alive_ids) = 0;
    virtual size_t vote(std::vector<size_t> alive_ids) {
        if (is_real_player) {
            return vote_ai(alive_ids);
        } else {
            return vote_ai(alive_ids);
        }
    }
    virtual size_t vote_ai(std::vector<size_t>& alive_ids) = 0;
    
    bool alive;
    bool is_real_player;
    size_t id;
    std::vector<size_t> known_mafia{};
    std::string team;
};

class Civilian : public Player {
public:
    Civilian(size_t id_p) : Player(id_p) {
        team = "civilian";
    }
    virtual ~Civilian() {};
    
    virtual size_t vote_ai(std::vector<size_t>& alive_ids) override {
        simple_shuffle(alive_ids);
        size_t i = 0;
        while (i < alive_ids.size()) {
            if (alive_ids[i] != id) {
                return alive_ids[i];
            }
            i++;
        }
        return 0;
    }
};

class Mafia : public Player {
public:
    Mafia(size_t id_p) : Player(id_p) {
        team = "mafia";
    }
    virtual ~Mafia() {};
    
    virtual size_t vote_ai(std::vector<size_t>& alive_ids) override {
        simple_shuffle(alive_ids);
        size_t i = 0;
        while (i < alive_ids.size()) {
            if (std::find(known_mafia.begin(), known_mafia.end(), alive_ids[i]) == known_mafia.end()) {
                return alive_ids[i];
            }
            i++;
        }
        return 0;
    }
};

class Maniac : public Player {
public:
    Maniac(size_t id_p) : Player(id_p) {
        team = "maniac";
    }
    virtual ~Maniac() {};
    
    virtual size_t vote_ai(std::vector<size_t>& alive_ids) override {
        simple_shuffle(alive_ids);
        size_t i = 0;
        while (i < alive_ids.size()) {
            if (alive_ids[i] != id) {
                return alive_ids[i];
            }
            i++;
        }
        return 0;
    }
};


class Game {
public:
    Logger* logger;
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
        logger = new Logger{"init.log"};
        logger->log(Loglevel::INFO, "--- INIT ---");
        std::vector<size_t> mafia_buf{};
        for (size_t i = 0; i < roles.size(); i++) {
            auto role = roles[i];
            if (role == "civilian") {
                logger->log(Loglevel::INFO,
                        TPrettyPrinter().Format("Player ").Format(i).Format(" is civilian").Str());
                players.push_back(Shared_pointer<Player>(new Civilian{i}));
            } else if (role == "mafia") {
                logger->log(Loglevel::INFO,
                        TPrettyPrinter().Format("Player ").Format(i).Format(" is mafia").Str());
                mafia_buf.push_back(i);
                players.push_back(Shared_pointer<Player>(new Mafia{i}));
            } else if (role == "maniac") {
                logger->log(Loglevel::INFO,
                        TPrettyPrinter().Format("Player ").Format(i).Format(" is maniac").Str());
                players.push_back(Shared_pointer<Player>(new Maniac{i}));
            } else if (role == "bull") {
                mafia_buf.push_back(i);
            } else if (role == "commissar") {
            } else if (role == "doctor") {
            } else if (role == "journalist") {
            } else if (role == "samurai") {
            }
        }
        for (auto i : mafia_buf) {
            players[i]->known_mafia.insert(players[i]->known_mafia.end(), mafia_buf.begin(),
                    mafia_buf.end());
        }
        delete logger;
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


    void main_loop() {
        unsigned int day_number = 0;
        bool break_flag = false;
        while (true) {
            logger = new Logger{"day_" + std::to_string(day_number) + ".log"};
            logger->log(Loglevel::INFO, "--- DAY " + std::to_string(day_number) + " ---");
            day_vote();
            auto cur_status = game_status();
            if (cur_status == "civilian") {
                std::cout << "civ" << std::endl;
                break_flag = true;
            } else if (cur_status == "mafia") {
                std::cout << "maf" << std::endl;
                break_flag = true;
            } else if (cur_status == "maniac") {
                std::cout << "mana" << std::endl;
                break_flag = true;
            }
            delete logger;
            day_number++;
            if (break_flag) {
                return;
            }
        }
    }
    void day_vote() {
        auto alives = players | view::filter([](auto p) { return p->alive; });
        auto alives_ids_rng = alives | view::transform([](auto p) { return p->id; });
        // decltype(players) alive_players
        std::vector<size_t> alives_ids{alives_ids_rng.begin(), alives_ids_rng.end()};
        std::map<size_t, unsigned int> votes{};
        for (auto id: alives_ids) {
            votes[id] = 0;
        }
        for (const auto& player : alives) {
            auto value = player->vote(alives_ids);
            votes[value]++;
            logger->log(Loglevel::INFO,
                    TPrettyPrinter().Format("Player ").Format(player->id).Format(" voted for player ").Format(value).Str());
        }
        auto key_val = std::max_element(votes.begin(), votes.end(),
                [](const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
                    return p1.second < p2.second;
                });
        players[key_val->first]->alive = false;
    }
    void night_act();
};


void debug(std::vector<std::string> v) {
    auto game = Game{10};
    game.init_players(v);
    print(game.game_status());
}

int main(void) {
    // std::srand(42);
    std::srand(time(NULL));
    auto game = Game(10);
    // print(game.get_random_roles());
    game.init_players({"mafia", "mafia", "mafia", "civilian", "civilian",
                       "civilian", "civilian", "civilian", "civilian", "maniac"});
    game.main_loop();
    // debug({}); // draw
    // debug({"civilian"}); // civ
    // debug({"civilian", "civilian"}); // civ
    // debug({"civilian", "maniac"}); // maniac
    // debug({"civilian", "civilian", "maniac"}); // cont
    // debug({"mafia", "civilian", "maniac"});
    // debug({"mafia", "civilian", "civilian"});
    // debug({"mafia", "mafia", "civilian"});
    return 0;
}
