from abc import ABC, abstractmethod
import numpy as np
import matplotlib.pyplot as plt


class QtnmBaseField(ABC):
    @abstractmethod
    def evaluate_field_at_point(self, x, y, z):
        pass

    def __sizes(self, x, y, z):
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

        bx = np.zeros(sizes[::-1])
        by = np.zeros_like(bx)
        bz = np.zeros_like(by)

        for k in np.arange(sizes[2]):
            for j in np.arange(sizes[1]):
                for i in np.arange(sizes[0]):
                    bx[k, j, i], by[k, j, i], bz[k, j, i] = \
                        self.evaluate_field_at_point(_x[i], _y[j], _z[k])

        return np.squeeze(bx), np.squeeze(by), np.squeeze(bz)

    def evaluate_field_magnitude(self, x, y, z):
        bx, by, bz = self.evaluate_field(x, y, z)
        return np.sqrt(bx**2 + by**2 + bz**2)

    def stream_plot(self, x, y, z, **kwargs):
        # Quick check on inputs
        sizes = self.__sizes(x, y, z)
        if np.sum(sizes == 1) != 1:
            raise ValueError('One of x,y,z must be scalar or size 1!')

        if sizes[0] == 1:
            U, V = np.meshgrid(y, z)
            _, bu, bv = self.evaluate_field(x, y, z)
        elif sizes[1] == 1:
            U, V = np.meshgrid(x, z)
            bu, _, bv = self.evaluate_field(x, y, z)
        else:
            U, V = np.meshgrid(x, y)
            bu, bv, _ = self.evaluate_field(x, y, z)

        return plt.streamplot(U, V, bu, bv, **kwargs)
