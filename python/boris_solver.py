"""
QTNM Boris method solver

"""

import numpy as np
from scipy.constants import electron_mass as me, elementary_charge as qe, c
from utils import calculate_omega

class BorisSolver():
    """ 
    Copy of the QTNM base while I figure out what is going on
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
        xPos = np.zeros(n_steps)
        yPos = np.zeros(n_steps)
        zPos = np.zeros(n_steps)
        xVel = np.zeros(n_steps)
        yVel = np.zeros(n_steps)
        zVel = np.zeros(n_steps)

        xPos[0] = x0[0]
        yPos[0] = x0[1]
        zPos[0] = x0[2]
        xVel[0] = v0[0]
        yVel[0] = v0[1]
        zVel[0] = v0[2]
        
        # Now loop through steps
        for step in range(1, n_steps):
            # Calculate first half of the position update
            gamma_n = 1 / np.sqrt( 1 - (np.linalg.norm(v_n)/c)**2 )
            x_nplushalf = x_n + u_n * step_size / (2.0 * gamma_n)

            # Inversion step
            # No electric fields so this is a lot simpler
            gamma_minus = np.sqrt( 1 + (np.linalg.norm(u_n)/c)**2 )
            t = self.calc_b_field(x_nplushalf[0], x_nplushalf[1], x_nplushalf[2]) * self.charge * step_size/(2.0 * self.mass * gamma_minus)
            s = 2.0 * t / (1.0 + np.linalg.norm(t)**2) 
            u_nplus1 = u_n + np.cross(u_n + np.cross(u_n, t), s)
            
            # Now update position
            gamma_nplus1 = np.sqrt( 1 + (np.linalg.norm(u_nplus1)/c)**2 )
            x_nplus1 = x_nplushalf + u_nplus1 * step_size / (2.0 * gamma_nplus1)
            v_nplus1 = u_nplus1 / gamma_nplus1

            # Update output arrays
            times[step] = step * step_size
            xPos[step] = x_nplus1[0]
            yPos[step] = x_nplus1[1]
            zPos[step] = x_nplus1[2]
            xVel[step] = v_nplus1[0]
            yVel[step] = v_nplus1[1]
            zVel[step] = v_nplus1[2]

            u_n = u_nplus1
            x_n = x_nplus1
            v_n = v_nplus1
            
        return times, xPos, yPos, zPos, xVel, yVel, zVel

    def acc(self, x, v):
        """
        Returns acceleration due B field

        Args:
            x: position vector
            v: velocity vector
        """

        dudt = self.charge / self.mass * ( np.cross(v, self.calc_b_field(x[0], x[1], x[2])) )
        gamma = 1.0 / np.sqrt( 1 - (np.linalg.norm(v)/c)**2 )
        acc = dudt / gamma
        
        return acc
    
