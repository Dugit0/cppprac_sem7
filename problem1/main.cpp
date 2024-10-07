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


struct NightActions {
    unsigned int players_num;
    bool doctors_action;
    unsigned int doctors_choice;
    bool commissar_action;
    unsigned int commissar_choice;
    bool journalist_action;
    std::pair<unsigned int, unsigned int> journalist_choice;
    bool samurai_action;
    unsigned int samurai_choice;
    std::vector<std::vector<unsigned int>> killers;
    explicit NightActions(unsigned int players_num_) : players_num(players_num_) {
        for (size_t i = 0; i < players_num; i++) {
            killers.push_back({});
        }
        commissar_action = false;
        doctors_action = false;
        journalist_action = false;
        samurai_action = false;
    }
    void reset() {
        for (size_t i = 0; i < killers.size(); i++) {
            killers[i].clear();
        }
        commissar_action = false;
        doctors_action = false;
        journalist_action = false;
        samurai_action = false;
    }
};


class Player {
public:
    Player(size_t id_p) : id(id_p) {
        alive = true;
        is_real_player = false;
        is_boss = false;
    };
    virtual ~Player() {};
    virtual size_t vote(std::vector<size_t> alive_ids) {
        if (is_real_player) {
            return vote_ai(alive_ids);
        } else {
            return vote_ai(alive_ids);
        }
    }
    virtual void act(std::vector<size_t> alive_ids,
                     NightActions& night_actions,
                     std::vector<Shared_pointer<Player>> players) {
        if (is_real_player) {
            act_ai(alive_ids, night_actions, players);
        } else {
            act_ai(alive_ids, night_actions, players);
        }
    }
    virtual size_t vote_ai(std::vector<size_t>& alive_ids) = 0;
    virtual void act_ai(std::vector<size_t>& alive_ids,
                        NightActions& night_actions,
                        std::vector<Shared_pointer<Player>> players) = 0;
    
    bool alive;
    bool is_real_player;
    bool is_boss;
    size_t id;
    std::vector<size_t> known_mafia{};
    std::string team;
    std::string role;
};

class Civilian : public Player {
public:
    Civilian(size_t id_p) : Player(id_p) {
        team = "civilian";
        role = "civilian";
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
    virtual void act_ai(std::vector<size_t>&, NightActions&, std::vector<Shared_pointer<Player>>) override {
        return;
    }
};

class Commissar : public Civilian {
    std::vector<size_t> already_checked;
    std::vector<size_t> known_civilian;
public:
    Commissar(size_t id_p) : Civilian(id_p) {
        already_checked = {id};
        known_civilian = {id};
        role = "commissar";
    }
    virtual ~Commissar() {};
    
    virtual size_t vote_ai(std::vector<size_t>& alive_ids) override {
        simple_shuffle(alive_ids);
        for (size_t i = 0; i < known_mafia.size(); i++) {
            if (std::find(alive_ids.begin(), alive_ids.end(), known_mafia[i]) != alive_ids.end()) {
                return known_mafia[i];
            }
        }
        return Civilian::vote_ai(alive_ids);
    }
    virtual void act_ai(std::vector<size_t>& alive_ids,
                        NightActions& night_actions,
                        std::vector<Shared_pointer<Player>> players) override {
        for (size_t i = 0; i < known_mafia.size(); i++) {
            if (std::find(alive_ids.begin(), alive_ids.end(), known_mafia[i]) != alive_ids.end()) {
                night_actions.killers[known_mafia[i]].push_back(id);
                return;
            }
        }
        auto max_id = *std::max_element(alive_ids.begin(), alive_ids.end());
        for (size_t i = 0; i <= max_id; i++) {
            if (std::find(already_checked.begin(), already_checked.end(), i) != already_checked.end()) {
                already_checked.push_back(i);
                if (players[i]->team == "mafia") {
                    known_mafia.push_back(i);
                } else {
                    known_civilian.push_back(i);
                }
                night_actions.commissar_action = true;
                night_actions.commissar_choice = i;
                return;
            }
        }
        return;
    }
};


class Doctor : public Civilian {
    size_t last_heal;
public:
    Doctor(size_t id_p) : Civilian(id_p) {
        role = "doctor";
        last_heal = std::numeric_limits<size_t>::max();
    }
    virtual ~Doctor() {}

    virtual void act_ai(std::vector<size_t>& alive_ids,
                        NightActions& night_actions,
                        std::vector<Shared_pointer<Player>>) override {
        simple_shuffle(alive_ids);
        for (size_t i = 0; i < alive_ids.size(); i++) {
            if (alive_ids[i] != last_heal) {
                night_actions.doctors_action = true;
                night_actions.doctors_choice = alive_ids[i];
                last_heal = alive_ids[i];
                return;
            }
        }
    }
};


class Journalist : public Civilian {
public:
    Journalist(size_t id_p) : Civilian(id_p) {
        role = "journalist";
    }
    virtual ~Journalist() {}

    virtual void act_ai(std::vector<size_t>& alive_ids,
                        NightActions& night_actions,
                        std::vector<Shared_pointer<Player>>) override {
        simple_shuffle(alive_ids);
        for (size_t i = 0; i < alive_ids.size() - 1; i++) {
            for (size_t j = i + 1; j < alive_ids.size(); j++) {
                if (i != id && j != id) {
                    night_actions.journalist_action = true;
                    night_actions.journalist_choice = {i, j};
                }
            }
        }
    }
};


class Samurai : public Civilian {
public:
    Samurai(size_t id_p) : Civilian(id_p) {
        role = "samurai";
    }
    virtual ~Samurai() {}

    virtual void act_ai(std::vector<size_t>& alive_ids,
                        NightActions& night_actions,
                        std::vector<Shared_pointer<Player>>) override {
        simple_shuffle(alive_ids);
        for (size_t i = 0; i < alive_ids.size(); i++) {
            night_actions.samurai_action = true;
            night_actions.samurai_choice = i;
        }
    }
};


class Mafia : public Player {
public:
    Mafia(size_t id_p) : Player(id_p) {
        team = "mafia";
        role = "mafia";
    }
    virtual ~Mafia() {}
    
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
    virtual void act_ai(std::vector<size_t>& alive_ids,
                        NightActions& night_actions,
                        std::vector<Shared_pointer<Player>>) override {
        if (is_boss) {
            simple_shuffle(alive_ids);
            size_t i = 0;
            while (i < alive_ids.size()) {
                if (std::find(known_mafia.begin(), known_mafia.end(), alive_ids[i]) == known_mafia.end()) {
                    night_actions.killers[alive_ids[i]].push_back(id);
                    return;
                }
                i++;
            }
        }
    }
};


class Bull : public Mafia {
public:
    Bull(size_t id_p) : Mafia(id_p) {
        role = "bull";
    }
    virtual ~Bull() {}
};


class Maniac : public Player {
public:
    Maniac(size_t id_p) : Player(id_p) {
        team = "maniac";
        role = "maniac";
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
    virtual void act_ai(std::vector<size_t>& alive_ids,
                        NightActions& night_actions,
                        std::vector<Shared_pointer<Player>>) override {
        simple_shuffle(alive_ids);
        size_t i = 0;
        while (i < alive_ids.size()) {
            if (alive_ids[i] != id) {
                night_actions.killers[alive_ids[i]].push_back(id);
                return;
            }
            i++;
        }
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
                        TPrettyPrinter().f("Player ").f(i).f(" is civilian").Str());
                players.push_back(Shared_pointer<Player>(new Civilian{i}));
            } else if (role == "mafia") {
                logger->log(Loglevel::INFO,
                        TPrettyPrinter().f("Player ").f(i).f(" is mafia").Str());
                mafia_buf.push_back(i);
                players.push_back(Shared_pointer<Player>(new Mafia{i}));
            } else if (role == "maniac") {
                logger->log(Loglevel::INFO,
                        TPrettyPrinter().f("Player ").f(i).f(" is maniac").Str());
                players.push_back(Shared_pointer<Player>(new Maniac{i}));
            } else if (role == "bull") {
                logger->log(Loglevel::INFO,
                        TPrettyPrinter().f("Player ").f(i).f(" is bull").Str());
                players.push_back(Shared_pointer<Player>(new Bull{i}));
                mafia_buf.push_back(i);
            } else if (role == "commissar") {
                logger->log(Loglevel::INFO,
                        TPrettyPrinter().f("Player ").f(i).f(" is commissar").Str());
                players.push_back(Shared_pointer<Player>(new Commissar{i}));
            } else if (role == "doctor") {
                logger->log(Loglevel::INFO,
                        TPrettyPrinter().f("Player ").f(i).f(" is doctor").Str());
                players.push_back(Shared_pointer<Player>(new Doctor{i}));
            } else if (role == "journalist") {
                logger->log(Loglevel::INFO,
                        TPrettyPrinter().f("Player ").f(i).f(" is journalist").Str());
                players.push_back(Shared_pointer<Player>(new Journalist{i}));
            } else if (role == "samurai") {
                logger->log(Loglevel::INFO,
                        TPrettyPrinter().f("Player ").f(i).f(" is samurai").Str());
                players.push_back(Shared_pointer<Player>(new Samurai{i}));
            }
        }
        for (auto i : mafia_buf) {
            players[i]->known_mafia.insert(players[i]->known_mafia.end(), mafia_buf.begin(),
                    mafia_buf.end());
        }
        simple_shuffle(mafia_buf);
        players[mafia_buf[0]]->is_boss = true;
        delete logger;
    }

    void reelection_mafia_boss() {
        auto mafia = players |
                     view::filter([](auto p) { return p->alive; }) |
                     view::filter([](auto p) { return p->team == "mafia"; });
        if (!mafia.empty() && 
                (mafia | view::filter([](auto p) { return p->is_boss; })).empty()) {
            std::vector<Shared_pointer<Player>> mafia_vec{mafia.begin(), mafia.end()};
            simple_shuffle(mafia_vec);
            mafia_vec[0]->is_boss = true;
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


    void main_loop() {
        unsigned int day_number = 0;
        while (true) {
            logger = new Logger{"day_" + std::to_string(day_number) + ".log"};
            logger->log(Loglevel::INFO, "--- DAY " + std::to_string(day_number) + " ---");
            day_vote();
            reelection_mafia_boss();
            auto cur_status = game_status();
            if (cur_status != "continue") {
                std::cout << cur_status << std::endl;
                delete logger;
                return;
            }
            night_act();
            reelection_mafia_boss();
            cur_status = game_status();
            if (cur_status != "continue") {
                std::cout << cur_status << std::endl;
                delete logger;
                return;
            }
            day_number++;
            delete logger;
        }
    }

    void day_vote() {
        auto alives = players | view::filter([](auto p) { return p->alive; });
        auto alives_ids_rng = alives | view::transform([](auto p) { return p->id; });
        std::vector<size_t> alives_ids{alives_ids_rng.begin(), alives_ids_rng.end()};
        std::map<size_t, unsigned int> votes{};
        for (auto id: alives_ids) {
            votes[id] = 0;
        }
        for (const auto& player : alives) {
            auto value = player->vote(alives_ids);
            votes[value]++;
            logger->log(Loglevel::INFO,
                    TPrettyPrinter().f("Player ").f(player->id).f(" voted for player ").f(value).Str());
        }
        auto key_val = std::max_element(votes.begin(), votes.end(),
                [](const std::pair<int, int>& p1, const std::pair<int, int>& p2) {
                    return p1.second < p2.second;
                });
        players[key_val->first]->alive = false;
        logger->log(Loglevel::INFO,
                TPrettyPrinter().f("Player ").f(key_val->first).f(" was executed by order of the city.").Str());
    }

    void night_act() {
        auto alives = players | view::filter([](auto p) { return p->alive; });
        auto alives_ids_rng = alives | view::transform([](auto p) { return p->id; });
        std::vector<size_t> alives_ids{alives_ids_rng.begin(), alives_ids_rng.end()};
        NightActions night_actions{players_num};
        night_actions.reset();
        for (const auto& player : alives) {
            player->act(alives_ids, night_actions, players);
        }

        std::cout << TPrettyPrinter().f(night_actions.killers).Str() << std::endl;
        for (size_t i = 0; i < players_num; i++) {
            if (!night_actions.killers[i].empty()) {
                auto log_message = TPrettyPrinter().f("Player ").f(i).f(" was killed by ").Str();
                for (size_t j = 0; j < night_actions.killers[i].size(); j++) {
                    log_message += players[night_actions.killers[i][j]]->role;
                    log_message += (j == night_actions.killers[i].size() - 1) ? "" : ", ";
                }
                players[i]->alive = false;
                logger->log(Loglevel::INFO, log_message);
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
    std::srand(time(NULL));
    auto game = Game(10);
    auto roles = game.get_random_roles();
    // print();
    game.init_players(roles);
    // game.init_players({"mafia", "mafia", "mafia", "civilian", "civilian",
    //                    "civilian", "civilian", "civilian", "civilian", "maniac"});
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
