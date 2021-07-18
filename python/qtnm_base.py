"""
QTNM base field module.

Provides the abstract classes, QtnmBaseField and QtnmBaseSolver.
New concrete implementations of this class should be compatible with
other python code within the Electron-Tracking package.
"""

from abc import ABC, abstractmethod
import numpy as np
import matplotlib.pyplot as plt
from scipy.constants import electron_mass as me, elementary_charge as qe
from scipy.integrate import solve_ivp
from utils import calculate_omega


class QtnmBaseSolver(ABC):

    def __init__(self, charge=-qe, mass=me, b_field=1.0, calc_b_field=None):

        self.mass = mass
        self.charge = charge
        self.calc_b_field = calc_b_field

        # If calc_b_field not provided, assume constant field, and store omega
        if calc_b_field is None:
            omega0 = calculate_omega(b_field, mass=mass, charge=charge)
            if np.size(omega0 == 3):
                self.omega0 = omega0
            elif np.size(omega0 == 1):
                self.omega0 = np.array([0,0,omega0], dtype=float)
            else:
                # Raise error here. TODO raise exception
                print('error')

    def get_omega(self, pos=np.zeros(3)):
        """
        Calculate omega as a function of position
        """

        # Use pre-calculated value if possible
        if self.calc_b_field is None:
            return self.omega0

        bfield = self.calc_b_field(pos)
        return calculate_omega(bfield, mass=self.mass, charge=self.charge, energy=0.0)

    @abstractmethod
    def rhs(self, t, x):
        """
        Return RHS of equation as a function of time(t) and x(vars solved for)
        """

    @abstractmethod
    def rhs_1d(self, t, x):
        """
        Return RHS of equation as a function of time(t) and x(vars solved for),
        assuming a one dimensional B-field (in z-direction)
        """

    def analytic_solution(self, t):
        """
        Return analytic solution as a function of t, assuming a uniform field
        """
        return None

    def analytic_solution_1d(self, t):
        """
        Return 1D (B = (0, 0, B_z), uniform) analytic solution as a function of t
        """
        return None

    def solve(self, n_rotations, x0=np.array([1.0, 0.0, 0.0]), v0=np.array([0.0, 1.0, 0.0]), ic=None, cfl=1e-3):
        """
        Numerically solve equation set for n_rotations.

        If RHS returns > 6 values the initial conditions are padded with zero

        Args:
            n_rotations: Number of gyro-orbits (as function of B(x0))
            x0: Initial position. Default: (1.0, 0.0, 0.0)
            v0: Initial position. Defailt: (0.0, 1.0, 0.0)
        """

        omega0 = self.get_omega(x0)

        # Maximum time step
        max_step = cfl / np.abs(omega0)

        # Final time
        t_end = n_rotations * 2.0 * np.pi / np.abs(omega0)

        # Check how many variables we're solving for. If > 6 pad initial conditions with zeros
        initial_conditions = np.append(x0, v0)
        n_additional_vars = np.size(self.rhs(0.0, initial_conditions)) - 6
        initial_conditions = np.append(initial_conditions, np.zeros(n_additional_vars))

        return solve_ivp(self.rhs, (0, t_end), initial_conditions, max_step=max_step)

    def solve_1d(self, n_rotations, x0=np.array([1.0, 0.0]), v0=np.array([0.0, 1.0]), cfl=1e-3):
        """
        Numerically solve equation set in 1D, for n_rotations

        If RHS returns > 4 values the initial conditions are padded with zero

        Args:
            n_rotations: Number of gyro-orbits (as function of B(x0))
            x0: Initial position. Scalar values interpretted as (x0, 0.0).
                Default: (1.0, 0.0)
            v0: Initial velocity. Scalar values interpretted as (0.0, v0).
                Default: (0.0, 1.0)
            cfl: Time-steps per gyro-orbit. Default: 1e-3
        """

        if np.size(v0) == 1:
            _v0 = np.array([0.0, v0])
        elif np.size(v0) == 2:
            _v0 = v0
        else:
            # Could possible fall back to 3D solve here? If np.size(x0 == 3)
            # TODO raise exception here
            print('error')

        if np.size(x0) == 1:
            _x0 = np.array([x0, 0.0])
        elif np.size(x0) == 2:
            _x0 = x0
        else:
            # TODO raise exception here
            print('error')

        # Caculate omega (t=0) to set end time and max dt
        omega0 = self.get_omega(np.append(_x0, 0.0))

        # Maximum time step
        max_step = cfl / np.abs(omega0)

        # Final time
        t_end = n_rotations * 2.0 * np.pi / np.abs(omega0)

        # Check how many variables we're solving for. If > 4 pad initial conditions with zeros
        initial_conditions = np.append(_x0, _v0)
        n_additional_vars = np.size(self.rhs_1d(0.0, initial_conditions)) - 4
        initial_conditions = np.append(initial_conditions, np.zeros(n_additional_vars))

        return solve_ivp(self.rhs_1d, (0, t_end), initial_conditions, max_step=max_step)

class QtnmBaseField(ABC):
    """
    QTNM Base field abstract class

    Methods
    -------
    evaluate_field_at_point(x, y, z): Evaluates the field at a given position.
                                      (abstractmethod)
    evaluate_field(x, y, z): Evaluate field for a set of points.
    evaluate_field_magnitude(x, y, z): Evaluate field magnitude for a set of
                                       points.
    stream_plot(x, y, z): Produce 2D magnetic field line plot.
    """
    @abstractmethod
    def evaluate_field_at_point(self, x, y, z):
        """
        Parameters
        ----------
        x: x position to evaluate field at.
        y: y position to evaluate field at.
        z: z position to evaluate field at.
        """

    @staticmethod
    def __sizes(x, y, z):
        return np.array([np.size(x), np.size(y), np.size(z)])

    def evaluate_field(self, x, y, z):
        """
        Parameters
        ----------
        x: x positions to evaluate field at (1D).
        y: y positions to evaluate field at (1D).
        z: z positions to evaluate field at (1D).
        """

        sizes = self.__sizes(x,  y, z)

        # If all three arguments are scalars, call evalute_field
        if np.all(sizes == 1):
            return self.evaluate_field_at_point(x, y, z)

        # Convert to arrays, so subscriptable
        _x = np.atleast_1d(x)
        _y = np.atleast_1d(y)
        _z = np.atleast_1d(z)

        # TODO handle multi-dimensional arrays
        # (e.g. those from meshgrid)

        b_x = np.zeros(sizes[::-1])
        b_y = np.zeros_like(b_x)
        b_z = np.zeros_like(b_y)

        for k in np.arange(sizes[2]):
            for j in np.arange(sizes[1]):
                for i in np.arange(sizes[0]):
                    b_x[k, j, i], b_y[k, j, i], b_z[k, j, i] = \
                        self.evaluate_field_at_point(_x[i], _y[j], _z[k])

        return np.squeeze(b_x), np.squeeze(b_y), np.squeeze(b_z)

    def evaluate_field_magnitude(self, x, y, z):
        """
        Parameters
        ----------
        x: x positions to evaluate field magnitude at (1D).
        y: y positions to evaluate field magnitude at (1D).
        z: z positions to evaluate field magnitude at (1D).
        """

        b_x, b_y, b_z = self.evaluate_field(x, y, z)

        return np.sqrt(b_x**2 + b_y**2 + b_z**2)

    def stream_plot(self, x, y, z, **kwargs):
        """
        Evaluate field in a plane and produce a 2D streamplot.
        Note that one, and only one of x, y, z must be size 1.
        The size 1 input argument is used to specify the plane to
        produce the streamplot in.

        Parameters
        ----------
        x: Position(s) to evaluate field at for use in streamplot
        y: Position(s) to evaluate field at for use in streamplot
        z: Position(s) to evaluate field at for use in streamplot
        **kwargs: **kwargs for matplotlib.pyplot.streamplot

        Raises
        ------
        ValueError If 1 (and only) one of x, y, z are size is not 1.
        """

        # Quick check on inputs
        sizes = self.__sizes(x, y, z)
        if np.sum(sizes == 1) != 1:
            raise ValueError('One of x,y,z must be scalar or size 1!')

        if sizes[0] == 1:
            grid = np.meshgrid(y, z)
            bfield = self.evaluate_field(x, y, z)[1:]
        elif sizes[1] == 1:
            grid = np.meshgrid(x, z)
            bfield = self.evaluate_field(x, y, z)[::2]
        else:
            grid = np.meshgrid(x, y)
            bfield = self.evaluate_field(x, y, z)[:-1]

        stream_plot = plt.streamplot(grid[0], grid[1], bfield[0], bfield[1],
                                     **kwargs)

        # Limit axes
        plt.xlim(np.min(grid[0]), np.max(grid[0]))
        plt.ylim(np.min(grid[1]), np.max(grid[1]))

        return stream_plot
