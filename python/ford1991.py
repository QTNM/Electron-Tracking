import numpy as np
from scipy.integrate import solve_ivp

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

def analytic_solution(t, omega, tau, vel0=1.0):
    """Calculate analytic solution for Ford & O'Connell equation

    Assumes that motion is initially vertical (at t=0), with magnitude vel0

    Args:
        t: Time(s) to calculation solution for
        omega: Cyclotron frequency
        tau: Larmor power parameter, such that P = tau * mass * a**2
        vel: Initial velocity magnitude

    Returns:
        Analytic Solution: [x, y, vx, vy]
    """

    mu = tau * omega**2

    vx_soln = (np.exp(-mu * t) * np.sin(omega*t)) / (1.0 + tau**2 * omega**2)
    vy_soln = (np.exp(-mu * t) * np.cos(omega*t)) / (1.0 + tau**2 * omega**2)

    x_soln = -np.exp(-mu * t) * (mu * np.sin(omega * t)
                                   + omega * np.cos(omega * t)) / ((omega**2 + mu**2)
                                                                   * (1 + tau**2 * omega**2))
    y_soln = np.exp(-mu * t) * (omega * np.sin(omega * t)
                                   - mu * np.cos(omega * t)) / ((omega**2 + mu**2)
                                                                * (1 + tau**2 * omega**2))
    # Scale results by initial velocity
    vx_soln *= vel0
    vy_soln *= vel0
    x_soln *= vel0
    y_soln *= vel0

    return x_soln, y_soln, vx_soln, vy_soln

def solve(n_rotations, omega, mass, tau, vel0=1.0):
    """Numerically solve Ford & O'Connell 1991 equation

    Assumes that motion is initially vertical (at t=0), with magnitude vel0
    Additionally solves for total radiated power as a function of time

    Args:
        n_rotations: Number of rotations to calculate
        omega: Cyclotron frequency
        mass: Mass of particle
        tau: Larmor power parameter, such that P = tau * mass * a**2
        vel: Initial velocity magnitude

    Returns:
        res: Numerical Solution
    """
    # Maximum timestep. Could probably be smaller
    max_step = 1e-3 / np.abs(omega)
    # Final time
    t_end = n_rotations * 2.0 * np.pi / np.abs(omega)

    # Set initial conditions
    x_init, y_init, vx_init, vy_init = analytic_solution(0, omega, tau, vel0=vel0)
    # Note that for tau /= 0, both x_init and y_init and non-zero
    # Final value is total radiated power
    ic = [x_init, y_init, vx_init, vy_init, 0.0]

    res = solve_ivp(rhs, (0, t_end), ic,
                    max_step=max_step, args=[omega, mass, tau])

    return res
