import matplotlib.pyplot as plt
import numpy as np

# A very basic error plotting routine
def error_plot(x, y, x_exact=None, y_exact=None, title=None, xlabel=None, ylabel=None):
    """Plot data and optionally compare to analytic solution

    Args:
        x: xdata
        y: ydata, as function of x
        x_exact: xdata for analytic solution. Default: None
        y_exact: Analytic solution, as a function of xdata. Default: None
        title: Plot title
        xlabel: xlabel for plot
        ylabel: ylabel for plot
    """

    # Plot data
    plt.plot(x, y)

    # Optionally plot analytic solution
    if y_exact is not None:
        # Use x_exact if present, otherwise assume y_exact = y_exact(x)
        if x_exact is not None:
            plt.plot(x_exact, y_exact, linestyle='--')
        else:
            plt.plot(x, y_exact, linestyle='--')

    plt.title(title, fontsize=14)
    plt.xlabel(xlabel, fontsize=12)
    plt.ylabel(ylabel, fontsize=12)

    # If all x positive, limit axis
    if np.all(x >= 0.0):
        plt.xlim(left=0.0)

    plt.tight_layout()
