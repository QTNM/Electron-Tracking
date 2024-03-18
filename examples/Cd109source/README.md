# Cd-109 source Example

This example Geant4 application models the action of a Cd-109 commercial source 
for the QTNM project, simulating a conversion electron source emission using Cd-109.

## Notes 

Set default physics list is EMStandard_opt4, high precision low-energy EM physics with
added Radioactive Decay Physics; G4ParticleGun for simple event generation, since only a Cd-109 Ion
needs to be placed, and radioactive decay takes over.

Geometry is a vacuum box containing a commercial source geometry from datasheets from Isotrak and QSA. A scoring
surface, a half-sphere around the source emission surface, is defined as a boundary between two vacuum spaces, 
purely for scoring of a free, emitted electron.

Scorer: scoring surface crossing - particle momentum vector and kinetic energy post-step and location. Output in ROOT file. 

## Build instruction

At Warwick, SCRTP, use cvmfs as the easiest environment setup (with bash):

source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc13-opt/setup.sh

Anyone using CentOS7 can source the following environment:

source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-centos7-gcc12-opt/setup.sh

which sets up Geant4 11.2 and GCC13(12) on a CentOS9(7) background. ROOT 6.30 will also be available. Just create a 'build' 
directory, then 

cd build; cmake ..; make

and run in the build directory for instance as 

./cd109source -m run.mac

Find out about CLI options using --help option.
