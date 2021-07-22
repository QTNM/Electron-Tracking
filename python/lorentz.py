"""
Lorentz equation particle tracking module.

Provides tracking of particles moving under Lorentz force.
Essentially an optimised version of Ford 1991 class, but with
tau remove from rhs routines to improve efficiency.

Analytic solution methods are inherited from Ford 1991 solutions,
but with tau = 0.0
"""

import numpy as np
from scipy.constants import electron_mass as me, elementary_charge as qe
from ford1991 import Ford1991Solver


class LorentzSolver(Ford1991Solver):

    def __init__(self, charge=-qe, mass=me, b_field=1.0, calc_b_field=None):
        super().__init__(charge=charge, mass=mass, b_field=b_field, calc_b_field=calc_b_field, tau=0.0)

    # RHS according to Lorentz equation. Non-relativistic
    def rhs_1d(t, x):
        """Calculate RHS for Lorentz equation

        Args:
            t: Time. Not used, but required for solve_ivp
            x: Current State [x, y, vx, vy, energy_radiated]

        Returns:
            Time derivatives: [vx, vy, ax, ay]
        """

        omega = self.get_omega(np.array([x[0], y[0], 0.0]))[2]

        # Calculate acceleration according to Lorentz
        accx = omega * x[3]
        accy = -omega * x[2]

        return [x[2], x[3], accx, accy]

    def rhs(t, x):
        """Calculate RHS for Lorentz equation in 3D

        Args:
            t: Time. Not used, but required for solve_ivp
            x: Current State [x, y, vx, vy, energy_radiated]

        Returns:
            Time derivatives: [vx, vy, vz, ax, ay, az]
        """

        omega = self.get_omega(x[:3])

        accx = omega[2] * x[4] - omega[1] * x[5]
        accy = omega[0] * x[5] - omega[2] * x[3]
        accz = omega[1] * x[3] - omega[0] * x[4]

        return x[3], x[4], x[5], accx, accy, accz
