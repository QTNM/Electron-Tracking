# Scattering Example

This example Geant4 application addresses the challenge on a suitable Physics List for 
the QTNM project with a view to explore the low-energy electron scattering in a thin Tritium gas.

Notes: set default physics list as G4EMStandardPhysics_option4; G4ParticleGun for simple event generation,
default is an electron with 18.575 keV, can change in macro; switch on simple uniform B-field in macro - default is none; 

Geometry is a vacuum box world with a cylinder filled with Hydrogen gas at target number density corresponding to 
10^14 per ccm; EM physics processes at low energy are independent of isotopes hence material is Hydrogen but 
density calculated as for H-3: 5x10^-10 g/ccm. A vacuum volume is at the end of the cylinder acting as stopwatch for 
scoring.

Scorers: (a) interactions in the gas - track ID, parent ID, kinetic energy and global time; (b) entering the stopwatch disk - 
track ID and global time. Output in ROOT file.

Scorers try two different methods, ID's only for first occurrence of a track; energy and time for every single step. Currently 
post-step kinetic energy scoring. 

## Build instruction

At Warwick, SCRTP, use cvmfs as the easiest environment setup (with bash):

source /cvmfs/sft.cern.ch/lcg/views/LCG_99/x86_64-centos7-gcc10-opt/setup.sh

which sets up Geant4 10.7 and GCC10 on a CentOS7 background. ROOT 6.22 will also be available. Just create a 'build' 
directory, then 

cd build; cmake ..; make

and run in the build directory for instance as 

./scattering -m run.mac

