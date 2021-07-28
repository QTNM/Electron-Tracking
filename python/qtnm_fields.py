"""
QTNM field module.

Provides concrete implementations of QtnmBaseField.

Implementations:
----------------
BiotSavart: General purpose numerical integration of current elements.
CoilField: Magnetic field due to a circular loop of wire.
BathTub: Magnetic field based on Project8 bathtub trap.
Solenoid: Approximate magnetic field due to solenoid.
"""

import numpy as np
from scipy.constants import mu_0 as mu0
from scipy.special import ellipk, ellipe
from qtnm_base import QtnmBaseField


class BiotSavart(QtnmBaseField):
    """
    Generic QTNM field class which numerically integrates the Biot-Savart law
    to produce a magnetic field.
    """

    def __init__(self, x, y, z, current=1.0, mu=mu0):
        """
        Parameters
        ----------
        x: x positions of current elements (1D)
        y: y positions of current elements (1D)
        z: z positions of current elements (1D)
        current: Current (Default 1.0).
        mu: Magnetic permeability (Default mu0)
        """

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


class CoilField(QtnmBaseField):
    """
    Interface to magnetic field generated by a circular loop of wire in xy
    plane.
    """

    def __init__(self, radius=0.005, current=40, Z=0.0, mu=mu0):
        """
        Parameters
        ----------
        radius: Radius of coil (Default 0.005)
        current: Current (Default 1.0).
        Z: Vertical position of coil (Default 1.0).
        mu: Magnetic permeability (Default mu0)
        """

        self.current = current
        self.mu = mu
        self.z = Z
        self.radius = radius

    def __central_field(self):
        return self.current * self.mu / self.radius / 2.0

    # On-Axis field
    def __on_axis_field(self, z):
        return (self.mu * self.current * self.radius**2 / 2.0
                / (self.radius**2 + (z - self.z)**2)**(1.5))

    def evaluate_field_at_point(self, x, y, z):
        rad = np.sqrt(x**2 + y**2)

        # If on-axis
        if rad / self.radius < 1e-10:
            return 0.0, 0.0, self.__on_axis_field(z)

        # z relative to position of coil
        z_rel = z - self.z

        b_central = self.__central_field()
        rad_norm = rad / self.radius
        z_norm = z_rel / self.radius
        alpha = (1.0 + rad_norm)**2 + z_norm**2
        root_alpha_pi = np.sqrt(alpha) * np.pi
        beta = 4 * rad_norm / alpha
        int_e = ellipe(beta)
        int_k = ellipk(beta)
        gamma = alpha - 4 * rad_norm

        b_r = b_central * (int_e * ((1.0 + rad_norm**2 + z_norm**2) / gamma)
                           - int_k) / root_alpha_pi * (z_rel / rad)
        b_z = b_central * (int_e * ((1.0 - rad_norm**2 - z_norm**2) / gamma)
                           + int_k) / root_alpha_pi

        return b_r * x / rad, b_r * y / rad, b_z


class BathTubField(QtnmBaseField):
    """
    Interface to magnetic field based on the Project 8 "bath tub" set-up.
    Essentially a superposition of a magnetic bottle formed by two current
    loops, and a background magnetic field.
    """

    def __init__(self, radius=0.005, current=40, Z1=-1, Z2=1,
                 background=np.zeros(3)):
        """
        Parameters
        ----------
        radius: Radius of current loops (Default 0.005)
        current: Current (Default 1.0).
        Z1: Vertical position of 1st coil (Default -1).
        Z2: Vertical position of 2nd coil (Default 1).
        background: Background magnetic field (Default 0).
        """

        self.coil1 = CoilField(radius=radius, current=current, Z=Z1)
        self.coil2 = CoilField(radius=radius, current=current, Z=Z2)
        self.background = background

    def evaluate_field_at_point(self, x, y, z):
        b_x1, b_y1, b_z1 = self.coil1.evaluate_field_at_point(x, y, z)
        b_x2, b_y2, b_z2 = self.coil2.evaluate_field_at_point(x, y, z)

        b_x = b_x1 + b_x2
        b_y = b_y1 + b_y2
        b_z = b_z1 + b_z2

        return np.array([b_x, b_y, b_z]) + self.background


class SolenoidField(QtnmBaseField):
    """
    Interface to approximate solenoid field.
    Solenoid is approximated by combining multiple instances of CoilField.
    """

    def __init__(self, radius=0.005, current=40, Zmin=-1, Zmax=1,
                 Ncoils=11):
        """
        Parameters
        ----------
        radius: Radius of current loops (Default 0.005)
        current: Current (Default 40).
        Zmin: Vertical position of 1st coil (Default -1).
        Zmax: Vertical position of last coil (Default 1).
        Ncoils: Number of coils to use (Default 11).
        """

        self.coils = []
        for z in np.linspace(Zmin, Zmax, Ncoils):
            self.coils.append(CoilField(radius=radius, current=current,
                                        Z=z))

    def evaluate_field_at_point(self, x, y, z):
        field = np.zeros(3)
        for coil in self.coils:
            field = field + coil.evaluate_field_at_point(x, y, z)

        return field
