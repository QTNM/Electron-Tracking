import matplotlib.pyplot as plt
import numpy as np
from scipy.constants import c, electron_mass as me, elementary_charge as qe


# A very basic error plotting routine
def error_plot(x, y, x_exact=None, y_exact=None, title=None, xlabel=None,
               ylabel=None):
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


def calculate_omega(b_field, charge=-qe, energy=0.0, mass=me):
    """ Calculate cyclotron frequency for a particle in a magnetic field

    Args:
        b_field: Magnetic Field (Tesla)
        charge: Particle charge. Default: electron charge
        energy: Particle kinetic energy (KeV). Default: 0.0
                (yields non-relativistic result)
        mass: Particle mass (Kg). Default: Electron mass
    """

    gamma_m0 = mass + 1e3 * energy * qe / c**2
    return charge * b_field / gamma_m0
