import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

import sys

if __name__ == "__main__":
    filepath = sys.argv[1]

    data = pd.read_csv(filepath)
    for dim in data["dim"].unique():
        data_dim = data[data["dim"] == dim]
        for metric in data["metric"].unique():
            data_dim_metric = data_dim[data_dim["metric"] == metric]

            grouped = data_dim_metric.groupby(["name", "N"]).mean(True).reset_index()
            grouped = grouped.pivot(index="N", columns="name", values='value') 
            grouped.plot(figsize=(11, 6))

            plt.loglog()
            plt.title(f"{metric}")
            plt.savefig(f"{metric}_{dim}.png")
            plt.close()