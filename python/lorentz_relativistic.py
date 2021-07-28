"""
Relativistic Lorentz equation particle tracking class.

Provides relativistic tracking of particles moving under Lorentz force.
Note: It is more efficient to use the non-relativistic Lorentz class,
with modified mass = gamma * m_0.
"""

import numpy as np
from scipy.constants import electron_mass as me, elementary_charge as qe, c
from ford1991 import Ford1991Solver


class LorentzSolverRelativistic(Ford1991Solver):

    def __init__(self, charge=-qe, mass=me, b_field=1.0, calc_b_field=None):
        super().__init__(charge=charge, mass=mass, b_field=b_field,
                         calc_b_field=calc_b_field, tau=0.0)

    @staticmethod
    def calculate_gamma_from_u(u):
        """Calculate gamma from u = gamma v

        Args:
            u: gamma v

        Returns:
            gamma: Relativistic Lorentz factor
        """

        return np.sqrt(1.0 + np.sum((u / c)**2))

    @staticmethod
    def calculate_gamma_from_v(v):
        """Calculate gamma from v

        Args:
            v: particle velocity

        Returns:
            gamma: Relativistic Lorentz factor
        """

        # Not safe for v -> c
        return 1.0 / np.sqrt(1.0 - np.sum((v / c)**2))

    # RHS according to Lorentz equation.
    def rhs_1d(self, t, x):

        """Calculate RHS for Lorentz equation in 1D.

        Args:
            t: Time. Not used, but required for solve_ivp
            x: Current State [x, y, ux, uy]

        Returns:
            Time derivatives: [vx, vy, ax, ay]
        """

        omega = self.get_omega(np.array([x[0], x[1], 0.0]))[2]

        gamma_rel = self.calculate_gamma_from_u(x[2:])
        v = x[2:] / gamma_rel

        # Calculate acceleration according to Lorentz
        accx = omega * v[1]
        accy = -omega * x[0]

        return [v[0], v[1], accx, accy]

    def rhs(self, t, x):
        """Calculate RHS for Lorentz equation in 3D

        Args:
            t: Time. Not used, but required for solve_ivp
            x: Current State [x, y, z, ux, uy, uz]

        Returns:
            Time derivatives: [vx, vy, vz, ax, ay, az]
        """

        gamma_rel = self.calculate_gamma_from_u(x[3:])
        vel = x[3:] / gamma_rel

        omega = self.get_omega(x[:3])
        acc = np.cross(x[3:], omega) / gamma_rel

        return vel[0], vel[1], vel[2], acc[0], acc[1], acc[2]

    def analytic_solution_1d(self, time, x0=np.array([1.0, 0.0]),
                             v0=np.array([0.0, 1.0])):
        """Calculate analytic solution for Lorentz equation in 1D.

        Thin wrapper to Ford 1991 solution, but with tau = 0.0, and mass
        replaced with gamma * mass

        Returns:
            Analytic Solution: [x, y, vx, vy]
        """

        gamma = self.calculate_gamma_from_v(np.append(v0, 0.0))
        mass = self.mass

        self.mass /= gamma
        soln = super().analytic_solution_1d(time, x0, v0)

        # Revert change to mass
        self.mass = mass

        return soln

    def analytic_solution(self, time, x0=np.array([1.0, 0.0, 0.0]),
                          v0=np.array([0.0, 1.0, 0.0])):
        """Calculate analytic solution for Lorentz equation in 3D

        Thin wrapper to Ford 1991 solution, but with tau = 0.0, and mass
        replaced with gamma * mass

        Returns:
            Analytic Solution: [x, y, z, vx, vy, vz]

        """

        gamma = self.calculate_gamma_from_v(v0)
        mass = self.mass

        self.mass /= gamma
        soln = super().analytic_solution(time, x0, v0)

        # Revert change to mass
        self.mass = mass

        return soln
