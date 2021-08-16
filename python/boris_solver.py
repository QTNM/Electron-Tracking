"""
QTNM Boris method solver

"""

import numpy as np
from scipy.constants import electron_mass as me, elementary_charge as qe, c
from utils import calculate_omega

class BorisSolver():
    """ 
    Copy of the QTNM base in the absence of a link to the base class
    
    Args:                                                                                            
        charge: Charge of the particle
        mass: Mass of particle                                                                     
        tau: Larmor power parameter, such that P = tau * mass * a**2                             
        calc_b_field: Method to calculate magnetic field as function                              
                      of (x, y, z)
    """
    
    def __init__(self, charge=-qe, mass=me, b_field=1.0, calc_b_field=None, tau=0.0):
        self.mass = mass
        self.charge = charge
        self.b_field = b_field
        self.calc_b_field = calc_b_field
        self.tau = tau

        if calc_b_field is not None:
            # Handle cases where calc_b_field returns a single component                             
            if np.size(calc_b_field(0, 0, 0)) == 1:
                self.calc_b_field = lambda x, y, z: \
                    np.array([0.0, 0.0, calc_b_field(x, y, z)])

        # If calc_b_field not provided, assume constant field and store omega
        if calc_b_field is None:
            omega0 = calculate_omega(b_field, mass=mass, charge=charge)
            if np.size(omega0) == 3:
                self.omega0 = omega0
            elif np.size(omega0) == 1:
                self.omega0 = np.array([0, 0, omega0], dtype=float)
            else:
                raise ValueError('Calculate omega returned erroneous size')

    def get_omega(self, pos=np.zeros(3)):
        """
        Calculate omega as a function of position
        """

        # Use pre-calculated value if possible
        if self.calc_b_field is None:
            return self.omega0

        bfield = self.calc_b_field(pos[0], pos[1], pos[2])
        return calculate_omega(bfield, mass=self.mass, charge=self.charge,
                               energy=0.0)

    def radiation_acceleration(self, pos, vel):
        """
        Calculate the acceleration from the radiation reaction force 
        Form is from Ford & O'Connell equation in 3D

        Args: 
            pos: 3D position array
            vel: 3D velocity array

        Returns:
            Array of accelerations
        """
        omega = self.get_omega(pos)

        denom = (1 + self.tau**2 * np.dot(omega, omega))
        acc = np.zeros(3)

        # Larmor terms
        acc[0] -= self.tau * (omega[2]**2 + omega[1]**2) * vel[0]
        acc[0] += self.tau * omega[0] * (omega[2] * vel[2] + omega[1] * vel[1])

        acc[1] -= self.tau * (omega[2]**2 + omega[0]**2) * vel[1]
        acc[1] += self.tau * omega[1] * (omega[2] * vel[2] + omega[0] * vel[0])

        acc[2] -= self.tau * (omega[0]**2 + omega[1]**2) * vel[2]
        acc[2] += self.tau * omega[2] * (omega[0] * vel[0] + omega[1] * vel[1])
        
        acc /= denom
        
        return acc
    
    
    def solve(self, n_rotations, x0=np.array([1.0, 0.0, 0.0]),
              v0=np.array([0.0, 1.0, 0.0]), cfl=1e-3):
        """
        Numerically solve equations using Boris method for n_rotations

        Args:                                                                                       
            n_rotations: Number of gyro-orbits (as function of B(x0))                                  
            x0: Initial position. Default: (1.0, 0.0, 0.0)                                          
            v0: Initial position. Default: (0.0, 1.0, 0.0)                                            

        Returns times, positions and velocities
        """

        # Calculate magnitude of omega                                                              
        omega0 = np.linalg.norm(self.get_omega(x0))

        # Maximum time step                                                                          
        max_step = cfl / omega0

        # Final time                                                                                
        t_end = n_rotations * 2.0 * np.pi / omega0

        # Calculate number of steps
        n_steps = int(np.ceil(t_end / max_step))
        step_size = t_end / n_steps

        u_n = v0 * 1 / np.sqrt( 1 - (np.linalg.norm(v0)/c)**2 )
        x_n = x0
        v_n = v0
        
        times = np.zeros(n_steps)
        pos   = np.zeros((3, n_steps))
        vel   = np.zeros((3, n_steps))

        pos[0][0] = x0[0]
        pos[1][0] = x0[1]
        pos[2][0] = x0[2]
        vel[0][0] = v0[0]
        vel[1][0] = v0[1]
        vel[2][0] = v0[2]
        
        # Now loop through steps
        for step in range(1, n_steps):
            # Calculate first half of the position update
            gamma_n = 1 / np.sqrt( 1 - (np.linalg.norm(v_n)/c)**2 )
            x_nplushalf = x_n + u_n * step_size / (2.0 * gamma_n)

            # Do the first half of the radiation reaction acceleration
            u_minus = u_n + (step_size / 2.0) * self.radiation_acceleration(x_nplushalf, u_n)
            gamma_minus = np.sqrt( 1 + (np.linalg.norm(u_n)/c)**2 )
            
            # Rotation step
            # No electric fields so this is a lot simpler
            t = self.calc_b_field(x_nplushalf[0], x_nplushalf[1], x_nplushalf[2]) * self.charge * step_size/(2.0 * self.mass * gamma_minus)
            s = 2.0 * t / (1.0 + np.linalg.norm(t)**2) 
            u_plus = u_minus + np.cross(u_minus + np.cross(u_minus, t), s)

            # Second half of the radiation reaction acceleration
            u_nplus1 = u_plus + (step_size / 2.0) * self.radiation_acceleration(x_nplushalf, u_plus)
            
            # Now update position
            gamma_nplus1 = np.sqrt( 1 + (np.linalg.norm(u_nplus1)/c)**2 )
            x_nplus1 = x_nplushalf + u_nplus1 * step_size / (2.0 * gamma_nplus1)
            v_nplus1 = u_nplus1 / gamma_nplus1

            # Update output arrays
            times[step] = step * step_size
            pos[0][step] = x_nplus1[0]
            pos[1][step] = x_nplus1[1]
            pos[2][step] = x_nplus1[2]
            vel[0][step] = v_nplus1[0]
            vel[1][step] = v_nplus1[1]
            vel[2][step] = v_nplus1[2]

            u_n = u_nplus1
            x_n = x_nplus1
            v_n = v_nplus1
            
        return times, pos, vel

    def acc(self, x, v):
        """
        Returns acceleration due B field and radiation losses

        Args:
            x: position vector
            v: velocity vector
        """

        omega = self.get_omega(x)

        # Lorentz force
        acc = np.cross(v, omega)

        # Larmor terms
        acc += self.radiation_acceleration(x, v)
        
        return acc
    
