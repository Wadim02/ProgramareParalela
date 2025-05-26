# data/gen_inputs.py
import random

def gen(n, p, fname):
    with open(fname, 'w') as f:
        f.write(f"{n} {p}\n")
        for _ in range(n):
            row = [str(random.randint(0,9)) for _ in range(n)]
            f.write(" ".join(row) + "\n")

# Configurații de test
configs = [
    (128, 10), (128, 100),
    (256, 10), (256, 100),
    (512, 10), (512, 100),
    (1024, 10), (1024, 100)
]

for n, p in configs:
    fname = f"input_{n}_{p}.txt"
    print(f"Generating {fname} …")
    gen(n, p, fname)
