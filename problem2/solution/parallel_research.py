from subprocess import check_output
from random import randint
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import numpy as np

def gen_input_data(num_proc, num_problems, min_dur, max_dur):
    with open(f"inputs/par_input.csv", "w") as f:
        print(num_proc, num_problems, sep=",", end=",\n", file=f)
        print(*[randint(min_dur, max_dur) for i in range(num_problems)],
              sep=",", end=",\n", file=f)

RESEARCH_SKIP = False
AVERAGE_STEPS = 1
MIN_PROC, MAX_PROC, STEP_PROC = 5, 100, 5
MIN_PROB, MAX_PROB, STEP_PROB = 10, 7500, 5
MIN_DUR, MAX_DUR = 50, 100


if not RESEARCH_SKIP:
    gen_input_data(50, 2000, 50, 100)
    for real_proc in map(int, np.linspace(1, 8, 4)):
        for avg in range(AVERAGE_STEPS):
            res = check_output(["./parallel_main.out", f"inputs/par_input.csv",
                                str(real_proc)],
                               text=True)
            print(res)
            duration = float(res.strip().split('\n')[-1].split()[1])

            for i, filename in enumerate(filenames):
                df = pd.DataFrame(raw_data[i])
                df.to_csv(filename, header=False, index=False)


# data = pd.read_csv('part_2.csv')

# plt.figure(figsize=(10, 6))
# plt.plot(data['Processes'], data['AverageTime(s)'], marker='o', linestyle='-')
# plt.xlabel('Число процессов')
# plt.ylabel('Время (с)')
# plt.title('Зависимость времени выполнения от числа процессов')
# plt.grid(True)
# plt.savefig('average_time_vs_processes.png')
# plt.close()

# plt.figure(figsize=(10, 6))
# plt.plot(data['Processes'], data['AverageCriterionK'], marker='o', linestyle='-')
# plt.xlabel('Число процессов')
# plt.ylabel('Критерий K')
# plt.title('Зависимость критерия K от числа процессов')
# plt.grid(True)
# plt.savefig('criterion_k_vs_processes.png')
# plt.close()
