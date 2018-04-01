import re
import numpy as np
import sys

with open(sys.argv[1], "r") as f:
    lines = f.readlines()


l = ("").join(lines).split("nthread")[1:]

for i in range(len(l)):
    time = re.findall(r"(\d+) ns", l[i])

    if l[i].find("differ") != -1:
        print(i, "error")
    mean = np.mean([int(x) for x in time]) / 1000000
    stddev = np.std([int(x) for x in time]) / 1000000
    print(l[i][:2].strip(), "{0:.4f}".format(mean), "{0:.4f}".format(stddev))

