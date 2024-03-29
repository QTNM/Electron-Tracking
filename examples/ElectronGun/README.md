# Electron Gun Example

This example Geant4 application models the action of an electron gun 
for the QTNM project, simulating electron scattering in a thin Helium gas.

## Notes 

Set default physics list as QTNMPhysicsList; G4ParticleGun for simple event generation,
default is an electron with 18.575 keV, can change in macro; 

Geometry is a vacuum box world with a cylinder, the pipe, filled with Helium gas at variable density (default STP: 1.66322e-4 g/ccm). 
The density can be changed with a macro command.

Scorer: interactions in the gas - particle momentum vector and kinetic energy post-step and deposited energy together
with event ID and track ID. Output in ROOT file. Added: store interaction location.

Added: macro commands for the event generator. Set mean and std deviation in energy, cooresponding to a realistic electron gun. 
Also, define a finite spot source, again as given in electron gun - a circle spot beam with diameter as input.

A simple ROOT analysis script is included to read from the file. Target question is the number of scattered 
electrons in a given angle range.

## Build instruction

At Warwick, SCRTP, use cvmfs as the easiest environment setup (with bash):

source /cvmfs/sft.cern.ch/lcg/views/LCG_99/x86_64-centos7-gcc10-opt/setup.sh

which sets up Geant4 10.7 and GCC10 on a CentOS7 background. ROOT 6.22 will also be available. Just create a 'build' 
directory, then 

cd build; cmake ..; make

and run in the build directory for instance as 

./egun -m run.mac

Find out about CLI options using --help option.
