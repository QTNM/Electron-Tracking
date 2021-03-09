import numpy as np
from scipy.integrate import solve_ivp

# RHS according to Ford & O'Connell (1991). Non-relativistic
def rhs(t, x, omega, mass, tau):
    accx = (omega * x[3] - tau * omega**2 * x[2]) / (1 + tau**2 * omega**2)
    accy = (-omega * x[2] - tau * omega**2 * x[3]) / (1 + tau**2 * omega**2)

    # Power according to Larmor formula
    power = tau * mass * (accx**2 + accy**2)
    return [x[2], x[3], accx, accy, power]

# Analytic solution. Assumes motion initially vertical
def analytic_solution(t, omega, tau):
    mu = tau * omega**2

    vx_soln = (np.exp(-mu * t) * np.sin(omega*t)) / (1.0 + tau**2 * omega**2)
    vy_soln = (np.exp(-mu * t) * np.cos(omega*t)) / (1.0 + tau**2 * omega**2)

    x_soln = -np.exp(-mu * t) * (mu * np.sin(omega * t)
                                   + omega * np.cos(omega * t)) / ((omega**2 + mu**2)
                                                                   * (1 + tau**2 * omega**2))
    y_soln = np.exp(-mu * t) * (omega * np.sin(omega * t)
                                   - mu * np.cos(omega * t)) / ((omega**2 + mu**2)
                                                                * (1 + tau**2 * omega**2))
    return x_soln, y_soln, vx_soln, vy_soln

# Solve Ford & O'Connell 1991 for n rotations
# Additionally solve for total radiated power as function of time
def solve(n_rotations, omega, mass, tau):
    # Maximum timestep. Could probably be smaller
    max_step = 1e-3 / np.abs(omega)
    # Final time
    t_end = n_rotations * 2.0 * np.pi / np.abs(omega)

    # Set initial conditions
    x_init, y_init, vx_init, vy_init = analytic_solution(0, omega, tau)
    # Note that for tau /= 0, both x_init and y_init and non-zero
    # Final value is total radiated power
    ic = [x_init, y_init, vx_init, vy_init, 0.0]

    res = solve_ivp(rhs, (0, t_end), ic,
                    max_step=max_step, args=[omega, mass, tau])

    return res
