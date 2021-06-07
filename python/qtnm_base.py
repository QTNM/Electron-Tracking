from abc import ABC, abstractmethod
import numpy as np
import matplotlib.pyplot as plt


class QtnmBaseField(ABC):
    @abstractmethod
    def evaluate_field_at_point(self, x, y, z):
        pass

    @staticmethod
    def __sizes(x, y, z):
        return np.array([np.size(x), np.size(y), np.size(z)])

    def evaluate_field(self, x, y, z):
        # Convert to arrays, so subscriptable
        _x = np.atleast_1d(x)
        _y = np.atleast_1d(y)
        _z = np.atleast_1d(z)

        # TODO handle multi-dimensional arrays
        # (e.g. those from meshgrid)

        sizes = self.__sizes(x,  y, z)

        # If all three arguments are scalars, call evalute_field
        if np.all(sizes == 1):
            return self.evaluate_field_at_point(_x, _y, _z)

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
        b_x, b_y, b_z = self.evaluate_field(x, y, z)
        return np.sqrt(b_x**2 + b_y**2 + b_z**2)

    def stream_plot(self, x, y, z, **kwargs):
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
            bfield= self.evaluate_field(x, y, z)[:-1]

        return plt.streamplot(grid[0], grid[1], bfield[0], bfield[1], **kwargs)
