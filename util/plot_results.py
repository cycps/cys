#!/usr/bin/env python3

import sys
import matplotlib.pyplot as plt
import pandas as pd
import matplotlib


if len(sys.argv) != 2:
    print('usage plot_results <results.csv>', file=sys.stderr)
    sys.exit(1)

matplotlib.style.use('ggplot')

df = pd.DataFrame.from_csv(sys.argv[1])
df.plot(); plt.show();
