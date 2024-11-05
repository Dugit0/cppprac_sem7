from subprocess import check_output
from random import randint


def gen_input_data(num_proc, num_problems, min_dur, max_dur):
    with open(f"inputs/input_{num_proc}_{num_problems}.csv", "w") as f:
        print(num_proc, num_problems, sep=",", end=",\n", file=f)
        print(*[randint(min_dur, max_dur) for i in range(num_problems)],
              sep=",", end=",\n", file=f)


MIN_PROC, MAX_PROC, STEP_PROC = 1, 100, 5
MIN_PROB, MAX_PROB, STEP_PROB = 10, 1000, 5
MIN_DUR, MAX_DUR = 50, 100

files = [
        open("outputs/iterations.csv", "w"),
        open("outputs/duration.csv", "w"),
        open("outputs/test.csv", "w")
        ]
for proc in range(MIN_PROC, MAX_PROC, (MIN_PROC + MAX_PROC) // STEP_PROC):
    for prob in range(MIN_PROB, MAX_PROB, (MIN_PROB + MAX_PROB) // STEP_PROB):
        gen_input_data(proc, prob, MIN_DUR, MAX_DUR)
        res = check_output(["./main.out", f"inputs/input_{proc}_{prob}.csv"],
                           text=True)
        print(proc, prob)
        print(res)
        for row, file in zip(res.split('\n'), files):
            _, value = row.split()
            print(f"{value},", end="", file=file)
    for file in files:
        print(file=file)

for file in files:
    file.close()
