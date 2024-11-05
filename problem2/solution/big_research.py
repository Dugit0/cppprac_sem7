from subprocess import check_output
from random import randint
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

def gen_input_data(num_proc, num_problems, min_dur, max_dur):
    with open(f"inputs/input_{num_proc}_{num_problems}.csv", "w") as f:
        print(num_proc, num_problems, sep=",", end=",\n", file=f)
        print(*[randint(min_dur, max_dur) for i in range(num_problems)],
              sep=",", end=",\n", file=f)

RESEARCH_SKIP = True
AVERAGE_STEPS = 2
MIN_PROC, MAX_PROC, STEP_PROC = 5, 100, 5
MIN_PROB, MAX_PROB, STEP_PROB = 10, 7500, 5
MIN_DUR, MAX_DUR = 50, 100


if not RESEARCH_SKIP:
    filenames = [
                 "outputs/iterations.csv",
                 "outputs/duration.csv",
                 "outputs/test.csv",
                ]
    # raw_csv = ["" for i in range(len(files))]
    raw_data = np.zeros((len(filenames), STEP_PROC, STEP_PROB))
    for i, proc in enumerate(np.linspace(MIN_PROC, MAX_PROC, STEP_PROC)):
        for j, prob in enumerate(np.linspace(MIN_PROB, MAX_PROB, STEP_PROB)):
            proc = int(proc)
            prob = int(prob)
            gen_input_data(proc, prob, MIN_DUR, MAX_DUR)
            for avg in range(AVERAGE_STEPS):
                res = check_output(["./main.out", f"inputs/input_{proc}_{prob}.csv"],
                                   text=True)
                print(proc, prob)
                print(res)
                for k, row in enumerate(res.strip().split('\n')):
                    _, value = row.split()
                    raw_data[k][i][j] += np.float64(value)
                print(raw_data)
    raw_data /= AVERAGE_STEPS
    print(raw_data)

    for i, filename in enumerate(filenames):
        df = pd.DataFrame(raw_data[i])
        df.to_csv(filename, header=False, index=False)

def create_heatmap(inp_filename, title, out_filename, cmap="Reds"):
    data = pd.read_csv(inp_filename, header=None)
    print(data)
    plt.figure(figsize=(10, 8))
    sns.heatmap(data,
                xticklabels=list(map(int, np.linspace(MIN_PROB, MAX_PROB, STEP_PROB))),
                yticklabels=list(map(int, np.linspace(MIN_PROC, MAX_PROC, STEP_PROC))),
                cmap=cmap,
                fmt=".2f",
                cbar=True,
                annot=True,
                )
    plt.title(title)
    plt.xlabel("Problems number")
    plt.ylabel("Process number")
    plt.gca().invert_yaxis()
    plt.savefig(out_filename, format='jpg', dpi=300)
    plt.close()

create_heatmap("outputs/duration.csv", "Cauchy duration", "heatmaps/cauchy_duration.jpg")
create_heatmap("outputs/iterations.csv", "Cauchy iterations", "heatmaps/cauchy_iterations.jpg", cmap="Blues")
# for algo, title, eps_filename in [
#     #('Boltzmann', 'Тепловая карта для алгоритма Больцмана', 'boltzmann_heatmap.eps'),
#     #('Cauchy', 'Тепловая карта для алгоритма Коши', 'cauchy_heatmap.eps'),
#     # ('Logarithmic Cauchy', 'Тепловая карта для логарифмического алгоритма Коши', 'log_cauchy_heatmap.eps')
#     ]:
#     create_heatmap(data, title, eps_filename)
