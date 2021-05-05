import numpy as np
from scipy.integrate import solve_ivp
from scipy.constants import electron_mass as me, elementary_charge as qe
from utils import calculate_omega


# RHS according to Ford & O'Connell (1991). Non-relativistic
def rhs(t, x, omega, mass, tau):
    """Calculate RHS for Ford & O'Connell equation

    Args:
        t: Time. Not used, but required for solve_ivp
        x: Current State [x, y, vx, vy, energy_radiated]
        omega: Cyclotron frequency
        mass: Mass of particle
        tau: Larmor power parameter, such that P = tau * mass * a**2

    Returns:
        Time derivatives: [vx, vy, ax, ay, radiated_power]
    """

    # Calculate acceleration according to Lorentz force and Larmor term
    accx = (omega * x[3] - tau * omega**2 * x[2]) / (1 + tau**2 * omega**2)
    accy = (-omega * x[2] - tau * omega**2 * x[3]) / (1 + tau**2 * omega**2)

    # Power according to Larmor formula
    power = tau * mass * (accx**2 + accy**2)
    return [x[2], x[3], accx, accy, power]


def analytic_solution(t, b_field=1.0, vel0=1.0, mass=me, charge=-qe, tau=0.0):
    """Calculate analytic solution for Ford & O'Connell equation

    Assumes that motion is initially vertical (at t=0), with magnitude vel0

    Args:
        t: Time(s) to calculation solution for
        b_field: Magnetic field. Assumed to be in z-direction. Default: 1.0
        vel0: Initial velocity. If scalar assumed to be in y-direction.
              Otherwise in xy plane. Default: 1.0
        mass: Mass of particle. Default: Electron mass
        charge: Charge of particle. Default: Electron charge
        tau: Larmor power parameter, such that P = tau * mass * a**2.
             Default: 0.0

    Returns:
        Analytic Solution: [x, y, vx, vy]
    """

    # Calculate non-relativistic omega
    omega = calculate_omega(b_field, charge=charge, energy=0.0, mass=mass)

    mu = tau * omega**2

    if np.size(vel0) == 1:
        phi = 0.0
        v0 = vel0
    else:
        if np.shape(vel0) == (2,):
            phi = np.arctan2(vel0[0], vel0[1])
            v0 = np.linalg.norm(vel0)
        else:
            raise ValueError("Vel0 may only be scalar, or 2D vector shape")

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
    vx_soln *= v0
    vy_soln *= v0
    x_soln *= v0
    y_soln *= v0

    return x_soln, y_soln, vx_soln, vy_soln


def solve(n_rotations, b_field=1.0, vel0=1.0, mass=me, charge=-qe, tau=0.0):
    """Numerically solve Ford & O'Connell 1991 equation

    Assumes that motion is initially vertical (at t=0), with magnitude vel0
    Additionally solves for total radiated power as a function of time

    Args:
        n_rotations: Number of rotations to calculate
        b_field: Magnetic field. Assumed to be in z-direction. Default: 1.0
        vel0: Initial velocity. If scalar assumed to be in y-direction.
              Otherwise in xy plane. Default: 1.0
        mass: Mass of particle. Default: Electron mass
        charge: Charge of particle. Default: Electron charge.
        tau: Larmor power parameter, such that P = tau * mass * a**2
             Default: 0.0

    Returns:
        res: Numerical Solution
    """

    # Calculate non-relativistic omega
    omega = calculate_omega(b_field, charge=charge, energy=0.0, mass=mass)

    # Maximum timestep. Could probably be smaller
    max_step = 1e-3 / np.abs(omega)
    # Final time
    t_end = n_rotations * 2.0 * np.pi / np.abs(omega)

    # Set initial conditions
    x_init, y_init, vx_init, vy_init = analytic_solution(0, b_field=b_field,
                                                         vel0=vel0, mass=mass,
                                                         charge=charge,
                                                         tau=tau)
    # Note that for tau /= 0, both x_init and y_init and non-zero
    # Final value is total radiated power
    ic = [x_init, y_init, vx_init, vy_init, 0.0]

    res = solve_ivp(rhs, (0, t_end), ic,
                    max_step=max_step, args=[omega, mass, tau])

    return res
