import numpy as np
from scipy.constants import electron_mass as me, elementary_charge as qe
from qtnm_base import QtnmBaseSolver
from utils import rotate_b_field, rotate_b_field_inverse, decompose_velocity


class Ford1991Solver(QtnmBaseSolver):

    def __init__(self, charge=-qe, mass=me, b_field=1.0, calc_b_field=None,
                 tau=0.0):
        super().__init__(charge=charge, mass=mass, b_field=b_field,
                         calc_b_field=calc_b_field)
        self.tau = tau

    def rhs(self, t, x):
        """Calculate RHS for Ford & O'Connell equation in 3D

        Args:
            t: Time. Not used, but required for solve_ivp
            x: Current State [x, y, vx, vy, energy_radiated]
            mass: Mass of particle
            tau: Larmor power parameter, such that P = tau * mass * a**2
            omega0: Pre-compyted cyclotron frequency (vector), to use if
                    calc_b_field is None
            calc_b_field: Method to calculate magnetic field as function
                          of (x, y, z)

        Returns:
            Time derivatives: [vx, vy, vz, ax, ay, az, radiated_power]
        """

        omega = self.get_omega(x[:3])

        denom = (1 + self.tau**2 * np.dot(omega, omega))
        acc = np.cross(x[3:], omega)

        # Now add on Larmor terms
        acc[0] -= self.tau * (omega[2]**2 + omega[1]**2) * x[3]
        acc[0] += self.tau * omega[0] * (omega[2] * x[5] + omega[1] * x[4])

        acc[1] -= self.tau * (omega[2]**2 + omega[0]**2) * x[4]
        acc[1] += self.tau * omega[1] * (omega[2] * x[5] + omega[0] * x[4])

        acc[2] -= self.tau * (omega[0]**2 + omega[1]**2) * x[5]
        acc[2] += self.tau * omega[2] * (omega[0] * x[3] + omega[1] * x[4])

        acc /= denom

        # Power according to Larmor formula
        power = self.tau * self.mass * np.sum(acc**2)

        return x[3], x[4], x[5], acc[0], acc[1], acc[2], power

    def rhs_1d(self, t, x):
        """Calculate RHS for Ford & O'Connell equation

        Args:
        t: Time. Not used, but required for solve_ivp
        x: Current State [x, y, vx, vy, energy_radiated]
        charge: Charge of particle
        mass: Mass of particle
        tau: Larmor power parameter, such that P = tau * mass * a**2
        omega0: Pre-compyted cyclotron frequency, to use if
                calc_b_field is None
        calc_b_field: Method to calculate magnetic field as function
                      of (x, y)

        Returns:
            Time derivatives: [vx, vy, ax, ay, radiated_power]
        """

        omega = self.get_omega(pos=np.array([x[0], x[1], 0.0]))[2]

        denom = 1.0 + self.tau**2 * omega**2

        # Calculate acceleration according to Lorentz force and Larmor term
        accx = (omega * x[3] - self.tau * omega**2 * x[2]) / denom
        accy = (-omega * x[2] - self.tau * omega**2 * x[3]) / denom

        # Power according to Larmor formula
        power = self.tau * self.mass * (accx**2 + accy**2)
        return [x[2], x[3], accx, accy, power]

    def analytic_solution(self, time, x0=np.array([1.0, 0.0, 0.0]),
                          v0=np.array([0.0, 1.0, 0.0])):
        """Calculate analytic solution for Ford & O'Connell equation in 3D

        Assumes a uniform magnetic field

        Args:
            time: Time(s) to calculation solution for
            x0: Inital position. Default: [1.0, 0.0, 0.0]
            v0: Initial velocity. Default: [0.0, 1.0, 0.0]

        Returns:
            Analytic Solution: [x, y, vx, vy]
        """

        # Calculate vpara, vperp
        vpar, vperp = decompose_velocity(v0, self.b_field)
        # Now rotate coordinate system so B aligned with z-axis
        mrot = rotate_b_field(self.b_field)
        # Velocity in rotated coordinate system
        vrot = np.dot(mrot, vperp)

        # Analytic 1D solution
        x_b, y_b, vx_b, vy_b = self.analytic_solution_1d(time, v0=vrot[:2])

        # Calculate full solution by transforming back and adding on vpara
        minv = rotate_b_field_inverse(self.b_field)

        # z_b = 0 for all time
        x_soln = minv[0, 0] * x_b + minv[0, 1] * y_b + vpar[0] * time
        y_soln = minv[1, 0] * x_b + minv[1, 1] * y_b + vpar[1] * time
        z_soln = minv[2, 0] * x_b + minv[2, 1] * y_b + vpar[2] * time
        vx_soln = minv[0, 0] * vx_b + minv[0, 1] * vy_b + vpar[0]
        vy_soln = minv[1, 0] * vx_b + minv[1, 1] * vy_b + vpar[1]
        vz_soln = minv[2, 0] * vx_b + minv[2, 1] * vy_b + vpar[2]

        # Shift solution to x0
        x_soln += (x0[0] - np.atleast_1d(x_soln)[0])
        y_soln += (x0[1] - np.atleast_1d(y_soln)[0])
        z_soln += (x0[2] - np.atleast_1d(z_soln)[0])

        return x_soln, y_soln, z_soln, vx_soln, vy_soln, vz_soln

    def analytic_solution_1d(self, time, x0=np.array([1.0, 0.0]),
                             v0=np.array([0.0, 1.0])):
        """Calculate analytic solution for Ford & O'Connell equation

        Assumes a uniform magnetic field, in the z-direction

        Args:
            time: Time(s) to calculation solution for.
            x0: Initial position. If scalar assumed to be x-coordinate.
                Otherwise in xy plane. Default: (1.0, 0.0)
            v0: Initial velocity. If scalar assumed to be in y-direction.
                  Otherwise in xy plane. Default: (0.0, 1.0)

        Returns:
            Analytic Solution: [x, y, vx, vy]
        """

        # Calculate non-relativistic omega
        omega = self.get_omega(np.append(x0, 0.0))

        # If 1D require 3rd component
        if np.size(self.b_field) == 1:
            omega = omega[2]
        else:
            # 3D solution. Take magnitude * sign(self.charge)
            omega_mag = np.linalg.norm(omega)
            if self.charge > 0:
                omega = omega_mag
            else:
                omega = -omega_mag

        if np.size(v0) == 1:
            _v0 = np.array([0.0, v0])
        elif np.size(v0) == 2:
            _v0 = v0
        else:
            raise ValueError("V0 cannot have size > 2")

        phi = np.arctan2(_v0[0], _v0[1])
        v0_mag = np.linalg.norm(_v0)

        mu = self.tau * omega**2

        phase = omega * time + phi
        # Factor of 1 / (1 + tau * mu) not needed as we scale solution such
        # that initial velocity is correct
        vx_soln = np.exp(-mu * time) * np.sin(phase)
        vy_soln = np.exp(-mu * time) * np.cos(phase)

        x_soln = -(vx_soln * mu + vy_soln * omega) / (omega**2 + mu**2)
        y_soln = (vx_soln * omega - vy_soln * mu) / (omega**2 + mu**2)

        # Scale results by initial velocity
        vx_soln *= v0_mag
        vy_soln *= v0_mag
        x_soln *= v0_mag
        y_soln *= v0_mag

        # Now correct for non-zero initial position
        if np.size(x0) == 1:
            _x0 = x0
            _y0 = 0.0
        else:
            _x0 = x0[0]
            _y0 = x0[1]

        x_soln += (_x0 - np.atleast_1d(x_soln)[0])
        y_soln += (_y0 - np.atleast_1d(y_soln)[0])

        return x_soln, y_soln, vx_soln, vy_soln
