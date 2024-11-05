#include <stdio.h>
#include <cstdio>
#include <assert.h>
#include <algorithm>
#include <random>
#include <vector>
#include <memory>
#include <ranges>
#include <cmath>
#include <chrono>

#include "sa.cpp"



int main(void) {
    // sa::MainLoop main_loop;
    // srand(42); // TODO Delete
    double START_TEMPERATURE = 100.0;
    FILE* f_inp = std::fopen("inputs/input.csv", "r");
    unsigned num_proc, num_prob;
    std::fscanf(f_inp, "%u,%u,\n", &num_proc, &num_prob);
    std::vector<unsigned> prob_lens(num_prob, 0);
    for (unsigned i = 0; i < num_prob; i++) {
        std::fscanf(f_inp, "%u,", &(prob_lens[i]));
    }
    std::fclose(f_inp);

    auto solution = std::make_shared<sa::Solution>(num_proc, prob_lens);
    solution->init_approximation();
    auto variation = sa::Variation();
    auto temp_law = sa::BoltzmannTemperature(START_TEMPERATURE);
    // auto temp_law = sa::CauchyTemperature(START_TEMPERATURE);
    // auto temp_law = sa::MixedTemperature(START_TEMPERATURE);
    auto main_loop = sa::MainLoop(solution, variation, temp_law);
    auto start = std::chrono::high_resolution_clock::now();
    auto best_solution = main_loop.start();
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = finish - start;
    std::cout << "Duration: " << duration.count() << std::endl;
    std::cout << "Test: " << best_solution->test() << std::endl;
    return 0;
}
