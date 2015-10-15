#!/usr/bin/env python3

import sys
import matplotlib.pyplot as plt
import pandas as pd
import matplotlib


if len(sys.argv) != 2:
    print('usage plot_results <results.csv>', file=sys.stderr)
    sys.exit(1)

matplotlib.style.use('ggplot')

df = pd.read_csv(
        sys.argv[1], 
        index_col=["t"], 
        usecols=["t", 
            "myhouse.m.e_fd",
#            "myhouse.m.i_fd",
            "myhouse.m.e_a",
            "myhouse.m.e_b",
            "myhouse.m.e_c",
            "myhouse.m.i_a",
            "myhouse.m.i_b",
            "myhouse.m.i_c"
#            "myhouse.m.l_aa",
#            "myhouse.m.l_bb",
#            "myhouse.m.l_cc",
#            "myhouse.m.l_ab",
#            "myhouse.m.l_bc",
#            "myhouse.m.l_ca",
#            "myhouse.m.l_afd",
#            "myhouse.m.l_akd",
#            "myhouse.m.l_akq"
        ]
        )

#df.plot(ylim=[-0.1, 0.1]); plt.show();
df.plot(); plt.show();
