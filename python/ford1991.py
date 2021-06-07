import numpy as np
from scipy.integrate import solve_ivp
from scipy.constants import electron_mass as me, elementary_charge as qe
from utils import calculate_omega
from utils import rotate_b_field, rotate_b_field_inverse, decompose_velocity


# RHS according to Ford & O'Connell (1991). Non-relativistic
def rhs(t, x, charge, mass, tau, omega0=1.0, calc_b_field=None):
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

    if calc_b_field is None:
        omega = omega0
    else:
        b = calc_b_field(x[0], x[1])
        omega = calculate_omega(b, charge=charge, energy=0.0, mass=mass)

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


def solve(n_rotations, b0=1.0, v0=1.0, mass=me, charge=-qe, tau=0.0,
          calc_b_field=None, ic=None, cfl=1e-3):
    """Numerically solve Ford & O'Connell 1991 equation

    Assumes that motion is initially vertical (at t=0), with magnitude vel0
    Additionally solves for total radiated power as a function of time

    Args:
        n_rotations: Number of rotations to calculate
        b_field: Magnetic field. Assumed to be in z-direction. Default: 1.0
                 If calc_b_field is None, this value is used for all time.
                 Otherwise, this value is only used for limiting the timestep.
        vel0: Initial velocity. If scalar assumed to be in y-direction.
              Otherwise in xy plane. Default: 1.0
        mass: Mass of particle. Default: Electron mass
        charge: Charge of particle. Default: Electron charge.
        tau: Larmor power parameter, such that P = tau * mass * a**2
             Default: 0.0
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
        # Note that for tau /= 0, both x_init and y_init and non-zero
        ic0 = analytic_solution(0, b_field=b0, vel0=v0, mass=mass,
                                charge=charge, tau=tau)

        # Also track total radiated power, so initialise to zero
        ic0 += (0.0,)
    else:
        ic0 = ic

    res = solve_ivp(rhs, (0, t_end), ic0,
                    max_step=max_step, args=[charge, mass, tau, omega0,
                                             calc_b_field])

    return res


# TODO. Is this optimal? As transform the initial velocity into the rotated
# frame we could just use vx', vy'.
# Extra cost of transforming back vz_b /= 0 > cost of velocity
# decomposition(?)
def analytic_solution_3d(time, b_field=np.array([0, 0, 1]),
                         vel0=np.array([0, 1, 0]), mass=me, charge=-qe,
                         tau=0.0):
    """Calculate analytic solution for Ford & O'Connell equation in 3D

    Assumes that motion is initially vertical (at t=0), with magnitude vel0

    Args:
        time: Time(s) to calculation solution for
        b_field: Magnetic field. Default: [0, 0, 1]
        vel0: Initial velocity. Default: [0, 1, 0]
        mass: Mass of particle. Default: Electron mass
        charge: Charge of particle. Default: Electron charge
        tau: Larmor power parameter, such that P = tau * mass * a**2.
             Default: 0.0

    Returns:
        Analytic Solution: [x, y, vx, vy]
    """

    # Calculate vpara, vperp
    vpar, vperp = decompose_velocity(vel0, b_field)
    # Now rotate coordinate system so B aligned with z-axis
    mrot = rotate_b_field(b_field)
    # Velocity in rotated coordinate system
    vrot = np.dot(mrot, vperp)

    # Calculate b-field magnitude for 1D solution
    bmag = np.linalg.norm(b_field)

    # Analytic 1D solution
    x_b, y_b, vx_b, vy_b = analytic_solution(time, b_field=bmag, vel0=vrot[0:2],
                                             mass=mass, charge=charge, tau=tau)

    # Calculate full solution by transforming back and adding on vpara
    minv = rotate_b_field_inverse(b_field)

    # z_b = 0 for all time
    x_soln = minv[0, 0] * x_b + minv[0, 1] * y_b + vpar[0] * time
    y_soln = minv[1, 0] * x_b + minv[1, 1] * y_b + vpar[1] * time
    z_soln = minv[2, 0] * x_b + minv[2, 1] * y_b + vpar[2] * time
    vx_soln = minv[0, 0] * vx_b + minv[0, 1] * vy_b + vpar[0]
    vy_soln = minv[1, 0] * vx_b + minv[1, 1] * vy_b + vpar[1]
    vz_soln = minv[2, 0] * vx_b + minv[2, 1] * vy_b + vpar[2]

    return x_soln, y_soln, z_soln, vx_soln, vy_soln, vz_soln


def rhs_3d(t, x, charge, mass, tau, omega0=np.array([0., 0., 1.]),
           calc_b_field=None):
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

    if calc_b_field is None:
        omega = omega0
    else:
        bfield = calc_b_field(x[0], x[1], x[2])
        omega = calculate_omega(bfield, charge=charge, energy=0.0, mass=mass)

    denom = (1 + tau**2 * np.dot(omega, omega))
    accx = omega[2] * x[4] - omega[1] * x[5] - tau * (omega[2]**2
                                                      + omega[1]**2) * x[3]
    accx += tau * omega[0] * (omega[2] * x[5] + omega[1] * x[4])
    accy = omega[0] * x[5] - omega[2] * x[3] - tau * (omega[2]**2
                                                      + omega[0]**2) * x[4]
    accy += tau * omega[1] * (omega[2] * x[5] + omega[0] * x[4])
    accz = omega[1] * x[3] - omega[0] * x[4] - tau * (omega[0]**2
                                                      + omega[1]**2) * x[5]
    accz += tau * omega[2] * (omega[0] * x[3] + omega[1] * x[4])
    accx /= denom
    accy /= denom
    accz /= denom

    # Power according to Larmor formula
    power = tau * mass * (accx**2 + accy**2 + accz**2)

    return x[3], x[4], x[5], accx, accy, accz, power


def solve_3d(n_rotations, b0, v0, mass=me, charge=-qe, tau=0.0,
             calc_b_field=None, ic=None, cfl=1e-3):
    """Numerically solve Ford & O'Connell 1991 equation in 3D

    Assumes that motion is initially vertical (at t=0), with magnitude vel0
    Additionally solves for total radiated power as a function of time

    Args:
        n_rotations: Number of rotations to calculate
        b_field: Magnetic field.
        vel0: Initial velocity.
        mass: Mass of particle. Default: Electron mass
        charge: Charge of particle. Default: Electron charge.
        tau: Larmor power parameter, such that P = tau * mass * a**2
             Default: 0.0
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
        return solve(n_rotations, b0, v0, mass, charge, tau)

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
                                   charge=charge, tau=tau)

        # Also track total radiated power, so initialise to zero
        ic0 += (0.0,)
    else:
        ic0 = ic

    res = solve_ivp(rhs_3d, (0, t_end), ic0,
                    max_step=max_step, args=[charge, mass, tau, wvec,
                                             calc_b_field])

    return res
