#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <random>
#include <vector>
#include <memory>
#include <ranges>
#include <cmath>


namespace sa {

namespace view = std::ranges::views;

template<typename T>
auto simple_choice(T container) {
    assert (!container.empty());
    std::mt19937 g(rand());
    std::shuffle(container.begin(), container.end(), g);
    return *container.begin();
}


class MainLoop {
public:
    MainLoop() {
        ;
    }

    void start() {
        ;
    }
};


class VSolution {
public:
    unsigned num_proc;
    std::vector<unsigned> prob_lens;
    std::vector<std::vector<unsigned>> table;
    virtual void init_approximation() = 0;
    virtual unsigned test() = 0;
    virtual std::shared_ptr<VSolution> copy() = 0;
    virtual void print_solution() = 0;

    virtual ~VSolution() {};

};


class VVariation {
public:
    virtual std::shared_ptr<VSolution> variation(VSolution& solution) = 0;
    virtual ~VVariation() {};
};


class VTemperature {
protected:
    double start_temperature;
public:
    double temperature;
    VTemperature(double t_temperature) {
        start_temperature = t_temperature;
        temperature = t_temperature;
    }
    virtual void change_temperature(unsigned i);
    virtual ~VTemperature() {};
};


// ================================================================================================


class Solution : public VSolution {
public:
    Solution(unsigned t_num_proc, std::vector<unsigned> t_prob_lens)
    {
        assert(t_num_proc > 0);
        num_proc = t_num_proc;
        prob_lens = t_prob_lens;
        table = std::vector(num_proc, std::vector<unsigned> {});
    }

    void init_approximation() override {
        for (unsigned i = 0; i < prob_lens.size(); i++) {
            table[0].push_back(prob_lens[i]);
        }
    }

    unsigned test() override {
        unsigned result = 0;
        for (unsigned proc = 0; proc < num_proc; proc++) {
            unsigned cur_value = 0;
            for (unsigned j = 0; j < table[proc].size(); j++) {
                cur_value += table[proc][j];
                result += cur_value;
            }
        }
        return result;
    }

    std::shared_ptr<VSolution> copy() override {
        auto new_solution = Solution(num_proc, prob_lens);
        new_solution.table = table;
        return std::make_shared<Solution>(new_solution);
    }

    void print_solution() override {
        for (unsigned proc = 0; proc < num_proc; proc++) {
            printf("Proc %u:", proc);
            if (table[proc].size() > 0) {
                for (unsigned i = 0; i < table[proc].size() - 1; i++) {
                    printf(" %u,", table[proc][i]);
                }
                printf(" %u", table[proc][table[proc].size() - 1]);
            }
            printf("\n");
        }
    }
};


class Variation : public VVariation {
public:
    std::shared_ptr<VSolution> variation(VSolution& solution) override {
        auto no_empty_proc = view::iota((unsigned) 0, solution.num_proc) |
                             view::filter([&](auto i) { return !solution.table[i].empty(); });
        unsigned proc = simple_choice(std::vector<unsigned>(no_empty_proc.begin(),
                                                            no_empty_proc.end()));
        auto new_proc_gen = view::iota((unsigned) 0, solution.num_proc) |
                            view::filter([&](auto i) { return i != proc; });
        unsigned new_proc = simple_choice(std::vector<unsigned>(new_proc_gen.begin(),
                                                                new_proc_gen.end()));
        auto problem = simple_choice(solution.table[proc]);
        auto new_solution = solution.copy();
        new_solution->table[proc].erase(std::find(new_solution->table[proc].begin(),
                                                 new_solution->table[proc].end(),
                                                 problem));
        auto index = rand() % (new_solution->table[new_proc].size() + 1);
        new_solution->table[new_proc].insert(new_solution->table[new_proc].begin() + index, problem);
        return new_solution;
    }
};


class BoltzmannTemperature : public VTemperature {
public:
    void change_temperature(unsigned i) override {
        temperature = start_temperature / std::log(1 + i);
    }
};


class CauchyTemperature : public VTemperature {
    void change_temperature(unsigned i) override {
        temperature = start_temperature / (1 + (double) i);
    }
};


class MixedTemperature : public VTemperature {
    void change_temperature(unsigned i) override {
        temperature = start_temperature * (std::log(1 + i) / (1 + (double) i));
    }
};

} // end namespace sa

int main(void) {
    // sa::MainLoop main_loop;
    sa::Solution solution(5, {3, 2, 3, 4});
    solution.init_approximation();
    auto var = sa::Variation();
    auto new_solution = var.variation(solution);
    solution.print_solution();
    std::cout << solution.test() << std::endl;
    std::cout << "=========" << std::endl;
    new_solution->print_solution();
    std::cout << new_solution->test() << std::endl;
    return 0;
}
