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
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "sa.cpp"



int main(int argc, char** argv) {
    if (argc < 3) {
        return 1;
    }
    double START_TEMPERATURE = 1000.0;
    unsigned MAX_ITER_WITHOUT_IMPR = 100;

    FILE* f_inp = std::fopen(argv[1], "r");
    unsigned num_proc, num_prob;
    std::fscanf(f_inp, "%u,%u,\n", &num_proc, &num_prob);
    std::vector<unsigned> prob_lens(num_prob, 0);
    for (unsigned i = 0; i < num_prob; i++) {
        std::fscanf(f_inp, "%u,", &(prob_lens[i]));
    }
    std::fclose(f_inp);

    char* chr_end{};
    unsigned NPROC = std::strtol(argv[2], &chr_end, 10);

    auto solution = std::make_shared<sa::Solution>(num_proc, prob_lens);
    solution->init_approximation();
    auto variation = sa::Variation();
    // auto temp_law = sa::BoltzmannTemperature(START_TEMPERATURE);
    auto temp_law = sa::CauchyTemperature(START_TEMPERATURE);
    // auto temp_law = sa::MixedTemperature(START_TEMPERATURE);

    auto best_solution = solution->copy();
    auto best_test = best_solution->test();

    auto start = std::chrono::high_resolution_clock::now();

    unsigned iter_without_imp = 0;
    while (iter_without_imp < MAX_ITER_WITHOUT_IMPR) {
        std::vector<pid_t> pids{};
        std::vector<int> socket_desc{};
        for (unsigned i = 0; i < NPROC; i++) {
            int sd[2];
            socketpair(AF_UNIX, SOCK_STREAM, 0, sd);

            pid_t pid = fork();
            if (pid == 0) {
                // son
                close(sd[0]);
                auto main_loop = sa::MainLoop(solution, variation, temp_law);
                auto best_solution = main_loop.start();
                best_solution->serialize(sd[1]);
                close(sd[1]);
                return 0;
            } else {
                close(sd[1]);
                pids.push_back(pid);
                socket_desc.push_back(sd[0]);
            }
        }
        for (unsigned i = 0; i < NPROC; i++) {
            // waitpid mb?
            auto son_solution = sa::deserialize(socket_desc[i]);
            close(socket_desc[i]);
            if (son_solution->test() < best_test) {
                best_test = son_solution->test();
                best_solution = son_solution;
                printf("New best_solution : %u\n", best_test);
                iter_without_imp = 0;
            }
        }
        int status;
        while (wait(&status) != -1) {}
        iter_without_imp++;
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = finish - start;
    std::cout << "Duration: " << duration.count() << std::endl;
    std::cout << "Test: " << best_solution->test() << std::endl;

    return 0;
}
