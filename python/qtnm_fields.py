import numpy as np
from scipy.constants import mu_0 as mu0
from qtnm_base import QtnmBaseField


class BiotSavart(QtnmBaseField):
    def __init__(self, x, y, z, current=1.0, mu=mu0):
        self.current = current
        self.mu = mu

        # Construct positions of current elements
        self.xc = 0.5 * (x[1:] + x[:-1])
        self.yc = 0.5 * (y[1:] + y[:-1])
        self.zc = 0.5 * (z[1:] + z[:-1])

        # Wire elements
        self.dlx = x[1:] - x[:-1]
        self.dly = y[1:] - y[:-1]
        self.dlz = z[1:] - z[:-1]

    def evaluate_field_at_point(self, x, y, z):
        # Displacement vectors
        r_x = x - self.xc
        r_y = y - self.yc
        r_z = z - self.zc
        rmag = np.sqrt(r_x**2 + r_y**2 + r_z**2)

        # Cross product components. Better implementations
        # (e.g. using built in cross product) exist
        lrx = self.dly * r_z - self.dlz * r_y
        lry = self.dlz * r_x - self.dlx * r_z
        lrz = self.dlx * r_y - self.dly * r_x

        b_x = np.sum(lrx / rmag**3)
        b_y = np.sum(lry / rmag**3)
        b_z = np.sum(lrz / rmag**3)

        b_x *= self.current * self.mu / 4.0 / np.pi
        b_y *= self.current * self.mu / 4.0 / np.pi
        b_z *= self.current * self.mu / 4.0 / np.pi

        return b_x, b_y, b_z


# TODO. This ought to just inherit from Biot-Savart
class CoilField(QtnmBaseField):
    def __init__(self, Ntheta, radius=0.005, current=40, Z=0.0, mu=mu0):
        self.current = current
        self.mu = mu

        phi = np.linspace(0.0, 2 * np.pi, Ntheta)
        x_coil = radius * np.cos(phi)
        y_coil = radius * np.sin(phi)

        # Midpoint of coil elements
        self.xm = 0.5 * (x_coil[1:] + x_coil[:-1])
        self.ym = 0.5 * (y_coil[1:] + y_coil[:-1])
        self.zm = Z

        # Coil elements, dl
        self.dlx = x_coil[1:] - x_coil[:-1]
        self.dly = y_coil[1:] - y_coil[:-1]

    # Might be nicer if this worked on an array of values
    def evaluate_field_at_point(self, x, y, z):
        r_x = x - self.xm
        r_y = y - self.ym
        r_z = z - self.zm
        rmag = np.sqrt(r_x**2 + r_y**2 + r_z**2)

        # Components of cross product
        lxrx = self.dly * r_z
        lxry = -self.dlx * r_z
        lxrz = self.dlx * r_y - self.dly * r_x

        b_x = np.sum(lxrx / rmag**3)
        b_y = np.sum(lxry / rmag**3)
        b_z = np.sum(lxrz / rmag**3)

        b_x *= self.current * self.mu / 4.0 / np.pi
        b_y *= self.current * self.mu / 4.0 / np.pi
        b_z *= self.current * self.mu / 4.0 / np.pi

        return b_x, b_y, b_z


class BathTubField(QtnmBaseField):
    def __init__(self, Ntheta, radius=0.005, current=40, Z1=-1, Z2=1,
                 background=np.zeros(3)):
        self.coil1 = CoilField(Ntheta, radius=radius, current=current, Z=Z1)
        self.coil2 = CoilField(Ntheta, radius=radius, current=current, Z=Z2)
        self.background = background

    def evaluate_field_at_point(self, x, y, z):
        b_x1, b_y1, b_z1 = self.coil1.evaluate_field_at_point(x, y, z)
        b_x2, b_y2, b_z2 = self.coil2.evaluate_field_at_point(x, y, z)

        return np.array([b_x1 + b_x2, b_y1 + b_y2, b_z1 + b_z2]) + self.background


class SolenoidField(QtnmBaseField):
    def __init__(self, Ntheta, radius=0.005, current=40, Zmin=-1, Zmax=1,
                 Ncoils=11):
        self.coils = []
        for z in np.linspace(Zmin, Zmax, Ncoils):
            self.coils.append(CoilField(Ntheta, radius=radius, current=current, Z=z))

    def evaluate_field_at_point(self, x, y, z):
        field = np.zeros(3)
        for c in self.coils:
            field = field + c.evaluate_field_at_point(x, y, z)

        return field
