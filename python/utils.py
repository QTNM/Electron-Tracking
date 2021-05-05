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
    return charge * np.array(b_field) / gamma_m0


def decompose_velocity(velocity, b_field):
    """ Decompose velocity into components parallel and perpendicular to
    magnetic field.

    Args:
        b_field: Magnetic Field
        velocity: Velocity
    """
    b_field = np.array(b_field)
    velocity = np.array(velocity)
    bmag2 = np.dot(b_field, b_field)
    vpar = np.dot(velocity, b_field) * b_field / bmag2
    vper = velocity - vpar
    return vpar, vper


def rotation_matrix(a, b):
    """ Calculate rotation matrix M, such that Ma is aligned to b

    Args:
        a: Initial vector direction
        b: Target direction
    """
    # np.allclose might be safer here
    if np.array_equal(a, b):
        return np.eye(3)

    if np.array_equal(a, -b):
        # TODO FIXME
        # Not the correct answer...
        return - np.eye(3)

    # Allow cases where a,b are not unit vectors, so normalise
    a = a / np.linalg.norm(a)
    b = b / np.linalg.norm(b)

    v = np.cross(a, b)
    s = np.linalg.norm(v)
    t = np.dot(a, b)
    vx = np.array([[0, -v[2], v[1]], [v[2], 0, -v[0]], [-v[1], v[0], 0]])
    return np.eye(3) + vx + np.dot(vx, vx) * (1-t) / (s**2)


def rotate_b_field(b_field):
    """ Rotate B field to be aligned with the z-direction

    Returns matrix M, such that M B = (0, 0, B_z)

    Args:
        b_field: Initial magnetic field vector
    """
    return rotation_matrix(b_field, np.array([0, 0, 1]))


def rotate_b_field_inverse(b_field):
    """ Rotate a z-directed B-field to be aligned with a target field direction

    Returns matrix M, such that M B_z is proportional to B

    Args:
        b_field: Target magnetic field direction
    """
    return rotation_matrix(np.array([0, 0, 1]), np.array(b_field))
