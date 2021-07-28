# Electron-Tracking for the QTNM Project in Python

A set of python modules for tracking the movement of electrons in magnetic fields.
The primary aim of the package is to offer the user an environment to test magnetic
traps and/or particle tracking algorithms, with minimal software pre-requisites.

The code is written and maintained by Tom Goffrey as part of the QTNM project.

## Functionality

The package provides a selection of magnetic fields and electron motion integration
classes, based on the the `QtnmBaseField` and `QtnmBaseSolver` respectively.

Concrete implementations of the `QtnmBaseSolver` class include:

 - `LorentzSolver` - Evolves the electron according to the Lorentz equation.
 - `Ford1991Solver` - Evolves the electron according to the Lorentz equation
 and the radiation reaction force as described by [Ford&O'Connell 1991](https://doi.org/10.1016/0375-9601(91)90054-C)

Concrete implementations of the `QtnmBaseField` class include:

 - `CoilField` - The magnetic field due to a loop of thin wire.
 - `BathTubField` - A magnetic field based on the "Bathtub" trap proposed by [Project 8](https://www.project8.org).
 - `SolenoidField` - An approximate solenoid field formed by combining multiple instances of `CoilField.`
 - `BiotSavart` - A generic magnetic field due to wire elements, computed using the Biot-Savart Law.

Several Jupyter Notebooks are provided to demonstrate the use of these features.

## Dependencies

The majority of the code can be run with only a small number of python dependencies:

 - Numpy
 - Scipy
 - Matplotlib (for visualisation)
 - abc (Abstract Base Classes)

Some of the example notebooks contain additional requirements, which are not
essential, but do increase the functionality of the package. A notable example is
the threading notebook, which requires:

 - concurrent.futures
 - functools

## FAQ

Q: What units does the code use?

A: Unless stated otherwise the code assumes SI units.

##

Q: What time-integration method is used?

A: The electron position and velocity are evolved using
`scipy.integrate.solve_ivp`. At the time of writing the default
solver is an explicit Runge-Kutta method of order 5(4), but concerned users
should consult the documentation for their version of scipy.

##

Q: What assumptions does the code make?

A: The answer to this question is module and class dependent, but some
notable assumptions are:

1. The magnetic fields are idealised fields calculated assuming a thin
wire, so will not be accurate in situations where the physical size of
the wire/coil is important.

2. The Ford 1991 solver class neglects the term in the radiation reaction
force due to the change in magnetic field along the electron path.

##

Q: I have a question that's not listed in the FAQ.

A: Please open an issue on the [issue tracker](https://github.com/QTNM/Electron-Tracking/issues).