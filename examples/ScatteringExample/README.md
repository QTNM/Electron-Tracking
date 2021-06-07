# Scattering Example

This example Geant4 application addresses the challenge on a suitable Physics List for 
the QTNM project with a view to explore the low-energy electron scattering in a thin Tritium gas.

## Notes 

Set default physics list as QTNMPhysicsList; G4ParticleGun for simple event generation,
default is an electron with 18.575 keV, can change in macro; switch on simple uniform B-field in macro - default is none; 

Change of physics list is possible by macro, naming list constructors. An example macro is included to switch to 
G4EMStandardPhysics_option3. 

Geometry is a vacuum box world with a cylinder filled with Hydrogen gas at number density corresponding to 
10^12 per ccm (to be checked); EM physics processes at low energy are independent of isotopes hence material is Hydrogen but 
density calculated as for H-3: 5x10^-12 g/ccm. A vacuum volume is at the end of the cylinder acting as stopwatch for 
scoring.

Scorers: (a) interactions in the gas - track ID, parent ID, kinetic energy pre- and post-step, deposited energy and global 
time; (b) entering the stopwatch disk - track ID and global time. Output in ROOT file.

Scorers try two different methods, ID's only for first occurrence of a track; energies and time for every single step. A 
simple ROOT analysis script is included to read from the file: 'shortsummary(filename)' simply counts interactions in the gas 
and hits at the stopwatch.

## Build instruction

At Warwick, SCRTP, use cvmfs as the easiest environment setup (with bash):

source /cvmfs/sft.cern.ch/lcg/views/LCG_99/x86_64-centos7-gcc10-opt/setup.sh

which sets up Geant4 10.7 and GCC10 on a CentOS7 background. ROOT 6.22 will also be available. Just create a 'build' 
directory, then 

cd build; cmake ..; make

and run in the build directory for instance as 

./scattering -m run.mac

Find out about CLI options using --help option.
