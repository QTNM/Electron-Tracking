import matplotlib.pyplot as plt
import numpy as np

# A very basic error plotting routine
def error_plot(x, y, x_exact=None, y_exact=None, title=None, xlabel=None, ylabel=None):
    plt.plot(x, y)

    if y_exact is not None:
        if x_exact is not None:
            plt.plot(x_exact, y_exact, linestyle='--')
        else:
            plt.plot(x, y_exact, linestyle='--')

    plt.title(title, fontsize=14)
    plt.xlabel(xlabel, fontsize=12)
    plt.ylabel(ylabel, fontsize=12)

    if np.all(x >= 0.0):
        plt.xlim(left=0.0)

    plt.tight_layout()
