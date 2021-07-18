import numpy as np
from scipy.constants import electron_mass as me, elementary_charge as qe
from qtnm_base import QtnmBaseSolver
from utils import rotate_b_field, rotate_b_field_inverse, decompose_velocity


class Ford1991Solver(QtnmBaseSolver):

    def __init__(self, charge=-qe, mass=me, b_field=1.0, calc_b_field=None, tau=0.0):
        super().__init__(charge=charge, mass=mass, b_field=b_field, calc_b_field=calc_b_field)
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
        accx = omega[2] * x[4] - omega[1] * x[5] - self.tau * (omega[2]**2
                                                      + omega[1]**2) * x[3]
        accx += self.tau * omega[0] * (omega[2] * x[5] + omega[1] * x[4])
        accy = omega[0] * x[5] - omega[2] * x[3] - self.tau * (omega[2]**2
                                                      + omega[0]**2) * x[4]
        accy += self.tau * omega[1] * (omega[2] * x[5] + omega[0] * x[4])
        accz = omega[1] * x[3] - omega[0] * x[4] - self.tau * (omega[0]**2
                                                      + omega[1]**2) * x[5]
        accz += self.tau * omega[2] * (omega[0] * x[3] + omega[1] * x[4])
        accx /= denom
        accy /= denom
        accz /= denom

        # Power according to Larmor formula
        power = self.tau * self.mass * (accx**2 + accy**2 + accz**2)

        return x[3], x[4], x[5], accx, accy, accz, power

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

        omega = self.get_omega(pos=np.array([x[0], x[1], 0.0]))

        # Calculate acceleration according to Lorentz force and Larmor term
        accx = (omega * x[3] - self.tau * omega**2 * x[2]) / (1 + self.tau**2 * omega**2)
        accy = (-omega * x[2] - self.tau * omega**2 * x[3]) / (1 + self.tau**2 * omega**2)

        # Power according to Larmor formula
        power = self.tau * self.mass * (accx**2 + accy**2)
        return [x[2], x[3], accx, accy, power]

    def analytic_solution(time, x0=np.array([1.0, 0.0, 0.0]), v0=np.array([0.0, 1.0, 0.0])):
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
        x_b, y_b, vx_b, vy_b = analytic_solution(time, x0=x0[:2], v0=v0[:2])

        # Calculate full solution by transforming back and adding on vpara
        minv = rotate_b_field_inverse(self.b_field)

        # z_b = 0 for all time
        x_soln = minv[0, 0] * x_b + minv[0, 1] * y_b + vpar[0] * time
        y_soln = minv[1, 0] * x_b + minv[1, 1] * y_b + vpar[1] * time
        z_soln = minv[2, 0] * x_b + minv[2, 1] * y_b + vpar[2] * time
        vx_soln = minv[0, 0] * vx_b + minv[0, 1] * vy_b + vpar[0]
        vy_soln = minv[1, 0] * vx_b + minv[1, 1] * vy_b + vpar[1]
        vz_soln = minv[2, 0] * vx_b + minv[2, 1] * vy_b + vpar[2]

        return x_soln, y_soln, z_soln, vx_soln, vy_soln, vz_soln

    def analytic_solution_1d(time, x0=np.array([1.0, 0.0]), v0=np.array([0.0, 1.0])):
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
        # Take magnitude as possible working with 3D field via
        # 3D analytic solution
        omega = np.linalg.norm(self.get_omega)

        if np.size(v0) == 1:
            _v0 = np.array(0.0, v0)
        elif np.size(v0) == 2:
            _v0 = v0
        else:
            raise ValueError("V0 cannot have size > 2")

        phi = np.arctan2(_v0[0], _v0[1])
        v0_mag = np.linalg.norm(_v0)

        self.mu = self.tau * omega**2

        phase = omega * t + phi
        # Factor of 1 / (1 + tau * mu) not needed, as we scale solution such that
        # initial velocity is correct
        vx_soln = np.exp(-mu * t) * np.sin(phase)
        vy_soln = np.exp(-mu * t) * np.cos(phase)

        x_soln = -np.exp(-mu * t) * ((mu * np.sin(phase) + omega * np.cos(phase))
                                 / (omega**2 + mu**2))
        y_soln = np.exp(-mu * t) * ((omega * np.sin(phase) - mu * np.cos(phase))
                                / (omega**2 + mu**2))

        # Scale results by initial velocity
        vx_soln *= v0_mag
        vy_soln *= v0_mag
        x_soln *= v0_mag
        y_soln *= v0_mag

        return x_soln, y_soln, vx_soln, vy_soln
