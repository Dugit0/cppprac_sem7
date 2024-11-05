from random import randint

num_proc, num_problems, min_len, max_len = map(int, input().split())
with open("inputs/input.csv", "w") as f:
    print(num_proc, num_problems, sep=",", end=",\n", file=f)
    print(*[randint(min_len, max_len) for i in range(num_problems)],
          sep=",", end=",\n", file=f)
