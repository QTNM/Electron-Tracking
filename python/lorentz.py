"""
Lorentz equation particle tracking module.

Provides tracking of particles moving under Lorentz force.
Essentially an optimised version of Ford 1991 module, but with
tau remove from rhs routines to improve efficiency.

Analytic solution methods are wrappers to Ford 1991 solutions,
but with tau = 0.0
"""

import numpy as np
from scipy.integrate import solve_ivp
from scipy.constants import electron_mass as me, elementary_charge as qe
from utils import calculate_omega
from ford1991 import analytic_solution as ford_soln
from ford1991 import analytic_solution_3d as ford_soln_3d


# RHS according to Lorentz equation. Non-relativistic
def rhs(t, x, charge, mass, omega0=1.0, calc_b_field=None):
    """Calculate RHS for Ford & O'Connell equation

    Args:
        t: Time. Not used, but required for solve_ivp
        x: Current State [x, y, vx, vy, energy_radiated]
        charge: Charge of particle
        mass: Mass of particle
        omega0: Pre-compyted cyclotron frequency, to use if
                calc_b_field is None
        calc_b_field: Method to calculate magnetic field as function
                      of (x, y)

    Returns:
        Time derivatives: [vx, vy, ax, ay]
    """

    if calc_b_field is None:
        omega = omega0
    else:
        b_field = calc_b_field(x[0], x[1])
        omega = calculate_omega(b_field, charge=charge, energy=0.0, mass=mass)

    # Calculate acceleration according to Lorentz
    accx = omega * x[3]
    accy = -omega * x[2]

    return [x[2], x[3], accx, accy]


def analytic_solution(t, b_field=1.0, vel0=1.0, mass=me, charge=-qe):
    """Calculate analytic solution for Lorentz equation

    Assumes that motion is initially vertical (at t=0), with magnitude vel0

    Wrapper to Ford 1991 analytic solution

    Args:
        t: Time(s) to calculation solution for
        b_field: Magnetic field. Assumed to be in z-direction. Default: 1.0
        vel0: Initial velocity. If scalar assumed to be in y-direction.
              Otherwise in xy plane. Default: 1.0
        mass: Mass of particle. Default: Electron mass
        charge: Charge of particle. Default: Electron charge

    Returns:
        Analytic Solution: [x, y, vx, vy]
    """

    return ford_soln(t, b_field, vel0, mass, charge, tau=0.0)


def solve(n_rotations, b0=1.0, v0=1.0, mass=me, charge=-qe,
          calc_b_field=None, ic=None, cfl=1e-3):
    """Numerically solve Lorentz equation

    Assumes that motion is initially vertical (at t=0), with magnitude vel0

    Args:
        n_rotations: Number of rotations to calculate
        b0: Magnetic field. Assumed to be in z-direction. Default: 1.0
                 If calc_b_field is None, this value is used for all time.
                 Otherwise, this value is only used for limiting the timestep.
        vel0: Initial velocity. If scalar assumed to be in y-direction.
              Otherwise in xy plane. Default: 1.0
        mass: Mass of particle. Default: Electron mass
        charge: Charge of particle. Default: Electron charge.
        calc_b_field: Method to calculate magnetic field as function
                      of (x, y)
        ic: Initial conditions. If None, the particle is initialised
            automatically, using vel0 (default behaviour).
        cfl: Orbital CFL number, such that the maximum timestep,
             dt_max = cfl / | omega |

    Returns:
        res: Numerical Solution
    """

    # Calculate non-relativistic omega(b0) to use as maximum timestep
    omega0 = calculate_omega(b0, charge=charge, energy=0.0, mass=mass)

    # Maximum timestep.
    max_step = cfl / np.abs(omega0)
    # Final time
    t_end = n_rotations * 2.0 * np.pi / np.abs(omega0)

    # Set initial conditions if not provided
    if ic is None:
        ic0 = analytic_solution(0, b_field=b0, vel0=v0, mass=mass,
                                charge=charge)
    else:
        ic0 = ic

    res = solve_ivp(rhs, (0, t_end), ic0,
                    max_step=max_step, args=[charge, mass, omega0,
                                             calc_b_field])

    return res


def analytic_solution_3d(time, b_field=np.array([0, 0, 1]),
                         vel0=np.array([0, 1, 0]), mass=me, charge=-qe):
    """Calculate analytic solution for Ford & O'Connell equation in 3D

    Assumes that motion is initially vertical (at t=0), with magnitude vel0

    Wrapper to Ford 1991 3D solution

    Args:
        time: Time(s) to calculation solution for
        b_field: Magnetic field. Default: [0, 0, 1]
        vel0: Initial velocity. Default: [0, 1, 0]
        mass: Mass of particle. Default: Electron mass
        charge: Charge of particle. Default: Electron charge

    Returns:
        Analytic Solution: [x, y, vx, vy]
    """

    return ford_soln_3d(time, b_field, vel0, mass, charge, tau=0.0)


def rhs_3d(t, x, charge, mass, omega0=np.array([0., 0., 1.]),
           calc_b_field=None):
    """Calculate RHS for Lorentz equation in 3D

    Args:
        t: Time. Not used, but required for solve_ivp
        x: Current State [x, y, vx, vy, energy_radiated]
        mass: Mass of particle
        omega0: Pre-compyted cyclotron frequency (vector), to use if
                calc_b_field is None
        calc_b_field: Method to calculate magnetic field as function
                      of (x, y, z)

    Returns:
        Time derivatives: [vx, vy, vz, ax, ay, az]
    """

    if calc_b_field is None:
        omega = omega0
    else:
        bfield = calc_b_field(x[0], x[1], x[2])
        omega = calculate_omega(bfield, charge=charge, energy=0.0, mass=mass)

    accx = omega[2] * x[4] - omega[1] * x[5]
    accy = omega[0] * x[5] - omega[2] * x[3]
    accz = omega[1] * x[3] - omega[0] * x[4]

    return x[3], x[4], x[5], accx, accy, accz


def solve_3d(n_rotations, b0, v0, mass=me, charge=-qe,
             calc_b_field=None, ic=None, cfl=1e-3):
    """Numerically solve Lorentz equation in 3D

    Assumes that motion is initially vertical (at t=0), with magnitude vel0

    Args:
        n_rotations: Number of rotations to calculate
        b0: Magnetic field.
        vel0: Initial velocity.
        mass: Mass of particle. Default: Electron mass
        charge: Charge of particle. Default: Electron charge.
        calc_b_field: Method to calculate magnetic field as function
                      of (x, y)
        ic: Initial conditions. If None, the particle is initialised
            automatically, using v0 (default behaviour).
        cfl: Orbital CFL number, such that the maximum timestep,
             dt_max = cfl / | omega |

    Returns:
        res: Numerical Solution
    """

    # Check if magnetic field is scalar, if so, pass to 1D solve
    if np.size(b0) == 1:
        return solve(n_rotations, b0, v0, mass, charge)

    # Calculate omega vector to calculate initial timestep
    wvec = calculate_omega(b0, charge=charge, energy=0.0, mass=mass)

    # Calculate max time step
    wmag = np.linalg.norm(wvec)
    max_step = cfl / wmag
    # Final time
    t_end = n_rotations * 2.0 * np.pi / wmag

    # Set initial conditions, if not provided by user
    if ic is None:
        # Note that for tau /= 0, both x_init and y_init and non-zero
        ic0 = analytic_solution_3d(0, b_field=b0, vel0=v0, mass=mass,
                                   charge=charge)
    else:
        ic0 = ic

    res = solve_ivp(rhs_3d, (0, t_end), ic0,
                    max_step=max_step, args=[charge, mass, wvec,
                                             calc_b_field])

    return res
