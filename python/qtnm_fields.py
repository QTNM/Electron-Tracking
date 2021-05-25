import numpy as np
from scipy.constants import mu_0 as mu0


class Coil_Field:
    def __init__(self, Ntheta, R=0.005, I=40, Z=0.0):
        self.R = R
        self.I = I

        phi = np.linspace(0.0, 2 * np.pi, Ntheta)
        Xcoil = R * np.cos(phi)
        Ycoil = R * np.sin(phi)

        # Midpoint of coil elements
        self.xm = 0.5 * (Xcoil[1:] + Xcoil[:-1])
        self.ym = 0.5 * (Ycoil[1:] + Ycoil[:-1])
        self.zm = Z

        # Coil elements, dl
        # Possibly sketchy sign convention. Chosen so that I>0 produces Bz > 0
        self.dlx = -(Xcoil[:-1] - Xcoil[1:])
        self.dly = -(Ycoil[:-1] - Ycoil[1:])

    # Might be nicer if this worked on an array of values
    def evaluate_field(self, x, y, z):
        rx = x - self.xm
        ry = y - self.ym
        rz = z - self.zm
        r = np.sqrt(rx**2 + ry**2 + rz**2)

        # Components of cross product
        lxrx = self.dly * rz
        lxry = -self.dlx * rz
        lxrz = self.dlx * ry - self.dly * rx

        bx = np.sum(lxrx / r**3)
        by = np.sum(lxry / r**3)
        bz = np.sum(lxrz / r**3)

        bx *= self.I * mu0 / 4.0 / np.pi
        by *= self.I * mu0 / 4.0 / np.pi
        bz *= self.I * mu0 / 4.0 / np.pi

        return bx, by, bz


class Bath_Tub_Field:
    def __init__(self, Ntheta, R=0.005, I=40, Z1=-1, Z2=1,
                 background=np.zeros(3)):
        self.c1 = Coil_Field(Ntheta, R=R, I=I, Z=Z1)
        self.c2 = Coil_Field(Ntheta, R=R, I=I, Z=Z2)
        self.background = background

    def evaluate_field(self, x, y, z):
        bx1, by1, bz1 = self.c1.evaluate_field(x, y, z)
        bx2, by2, bz2 = self.c2.evaluate_field(x, y, z)

        return np.array([bx1 + bx2, by1 + by2, bz1 + bz2]) + self.background
