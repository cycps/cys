#!/usr/bin/env python3

import sys
import matplotlib.pyplot as plt
import pandas as pd
import matplotlib


if len(sys.argv) != 2:
    print('usage plot_results <results.csv>', file=sys.stderr)
    sys.exit(1)

matplotlib.style.use('ggplot')

df = pd.read_csv(sys.argv[1], index_col=["t"], usecols=["t", "myhouse.m.theta", "myhouse.m.w", "myhouse.m.w'"])
df.plot(ylim=[-10, 500]); plt.show();
