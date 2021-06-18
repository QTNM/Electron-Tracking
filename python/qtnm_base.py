"""
QTNM base field module.

Provides the abstract class, QtnmBaseField.
New concrete implementations of this class should be compatible with
other python code within the Electron-Tracking package.
"""

from abc import ABC, abstractmethod
import numpy as np
import matplotlib.pyplot as plt


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

        pass

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
