import re
import numpy as np
import sys

with open(sys.argv[1], "r") as f:
    lines = f.readlines()


l = ("").join(lines).split("nthread")[1:]


import pandas as pd

res = {"n_thread":[], "mean": [], "std": []}


for i in range(len(l)):
    time = re.findall(r"(\d+) ms", l[i])

    if l[i].find("differ") != -1:
        print(i, "error")
    res["n_thread"].append(l[i][:2].strip())
    res["mean"].append(np.mean([int(x) for x in time]))
    res["std"].append(np.std([int(x) for x in time]))

df = pd.DataFrame(res)
df = df[["n_thread", "mean", "std"]]
df.to_csv(sys.argv[1].split('.')[0] + ".csv", index = False)


