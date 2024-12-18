#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <random>
#include <time.h>
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


class VSolution {
public:
    unsigned num_proc;
    std::vector<unsigned> prob_lens;
    std::vector<std::vector<unsigned>> table;
    virtual void init_approximation() = 0;
    virtual unsigned test() = 0;
    virtual std::shared_ptr<VSolution> copy() = 0;
    virtual void serialize(int fd) = 0;
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
    virtual void change_temperature(unsigned i) = 0;
    virtual ~VTemperature() {};
};

using VSolutionPtr = std::shared_ptr<VSolution>;

class MainLoop {
public:
    VSolutionPtr solution;
    VVariation& variation;
    VTemperature& temp_law;
    MainLoop(std::shared_ptr<VSolution> t_solution, VVariation& t_variation, VTemperature& t_temp_law)
        : solution(t_solution)
        , variation(t_variation)
        , temp_law(t_temp_law)
    {
        printf("%b, %b\n", time(nullptr), (getpid()<<16)); fflush(stdout);
        srand(time(nullptr) ^ (getpid()<<16));
    }

    VSolutionPtr start() {
        // printf("Start random %d\n", rand());
        unsigned MAX_ITER = 100;

        auto best_solution = solution->copy();
        auto cur_quality = best_solution->test();

        unsigned cur_iteration = 0;
        unsigned iter_without_improve = 0;
        while (true) {
            // printf("----\nIt = %u\n", cur_iteration);
            auto new_solution = solution->copy();
            new_solution = variation.variation(*new_solution);
            // printf("New variation:\n");
            // new_solution->print_solution();
            // printf("%u, %u", new_solution->test(), solution->test());
            long long delta = (long long) new_solution->test() - (long long) solution->test();
            // printf("delta = %lld\n", delta); fflush(stdout);
            if (delta <= 0) {
                // printf("d <= 0\n"); fflush(stdout);
                solution = new_solution;
            } else {
                long double p = std::exp(- (long double) delta / temp_law.temperature);
                // printf("d > 0, p = %Lf\n", p); fflush(stdout);
                if (((long double) rand()) / RAND_MAX <= p) {
                    // printf("p-mutation\n"); fflush(stdout);
                    solution = new_solution;
                }
            }
            // printf("New solution:\n");
            // solution->print_solution();
            // printf("cur_quality = %u, solution.test = %u\n", cur_quality, solution->test());
            if (cur_quality > solution->test()) {
                // printf("Improve %u | %u | %u\n",
                //         cur_quality,
                //         solution->test(),
                //         cur_iteration);
                // fflush(stdout);
                best_solution = solution;
                cur_quality = best_solution->test();
                iter_without_improve = 0;
            } else {
                iter_without_improve++;
                // printf("Not improve (%u)\n", iter_without_improve);
            }
            if (iter_without_improve > MAX_ITER) {
                // printf("Iterations: %u\n", cur_iteration);
                // printf("Best solution: %u\n", best_solution->test());
                return best_solution;
            }
            cur_iteration++;
            temp_law.change_temperature(cur_iteration);
        }
    }
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
            unsigned proc = rand() % num_proc;
            table[proc].push_back(prob_lens[i]);
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

    void serialize(int fd) override {
        write(fd, &(num_proc), sizeof(num_proc));
        size_t prob_lens_size = prob_lens.size();
        write(fd, &prob_lens_size, sizeof(prob_lens_size));
        for (const auto& len: prob_lens) {
            write(fd, &len, sizeof(len));
        }
        size_t table_size = table.size();
        write(fd, &table_size, sizeof(table_size));
        for (const auto& proc : table) {
            size_t proc_size = proc.size();
            write(fd, &proc_size, sizeof(proc_size));
            for (const auto& i : proc) {
                write(fd, &i, sizeof(i));
            }
        }
    }


    std::shared_ptr<VSolution> copy() override {
        auto new_solution = std::make_shared<Solution>(num_proc, prob_lens);
        new_solution->table = table;
        return new_solution;
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


VSolutionPtr deserialize(int fd) {
    unsigned num_proc;
    read(fd, &(num_proc), sizeof(num_proc));
    size_t prob_lens_size;
    read(fd, &prob_lens_size, sizeof(prob_lens_size));
    std::vector<unsigned> prob_lens(prob_lens_size, 0);
    for (size_t i = 0; i < prob_lens_size; i++) {
        read(fd, &(prob_lens[i]), sizeof(prob_lens[i]));
    }

    size_t table_size;
    read(fd, &table_size, sizeof(table_size));
    std::vector<std::vector<unsigned>> table(table_size, std::vector<unsigned>{});
    for (size_t i = 0; i < table_size; i++) {
        size_t proc_size;
        read(fd, &proc_size, sizeof(proc_size));
        for (size_t j = 0; j < proc_size; j++) {
            unsigned prob;
            read(fd, &prob, sizeof(prob));
            table[i].push_back(prob);
        }
    }
    VSolutionPtr new_solution = std::make_shared<Solution>(num_proc, prob_lens);
    new_solution->table = table;
    return new_solution;
}


class Variation : public VVariation {
public:
    VSolutionPtr variation(VSolution& solution) override {
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
    BoltzmannTemperature(double t_temperature) : VTemperature(t_temperature) {};
    void change_temperature(unsigned i) override {
        temperature = start_temperature / std::log(1 + i);
    }
};


class CauchyTemperature : public VTemperature {
public:
    CauchyTemperature(double t_temperature) : VTemperature(t_temperature) {};
    void change_temperature(unsigned i) override {
        temperature = start_temperature / (1 + (double) i);
    }
};


class MixedTemperature : public VTemperature {
public:
    MixedTemperature(double t_temperature) : VTemperature(t_temperature) {};
    void change_temperature(unsigned i) override {
        temperature = start_temperature * (std::log(1 + i) / (1 + (double) i));
    }
};


// ================================================================================================



} // end namespace sa
