# Cd-109 source Example

This example Geant4 application models the action of a Cd-109 commercial source 
for the QTNM project, simulating a conversion electron source emission using Cd-109.

## Notes 

Default physics list is EMStandard_opt4, high precision low-energy EM physics with
added Radioactive Decay Physics; G4ParticleGun for simple event generation, since only a Cd-109 Ion
needs to be placed, and radioactive decay takes over.

Geometry is a vacuum sphere containing a commercial source geometry from datasheets from Isotrak
and QSA. A macro command is available to switch between the two alternatives. The Isotrak
source is set as default. The datasheet does not specify how the Cd-109 source is deposited
except for the 5mm radius circle. The description specifies the source material being
glued between two Mylar foils of thickness 0.9mg/cm2. This is hence treated as a volume
emitter, with the Cd-109 ions started in the middle of a 100 nm Cd layer.

The QSA source is nearly identical except here the 5mm source circular deposit is described
as deposited on a Nickel backing of unknown thickness. An acrylic window of 100-200 mum/cm2
thickness covers that surface. This is therefore taken as an example of a surface source, i.e.
the Cd-109 ions are set on the surface of the Nickel volume.

A scoring surface, a half-sphere on top of the source emission surface, is defined as a 
boundary between two vacuum spaces, purely for scoring of a free, emitted electron.

Scorer: scoring surface crossing - particle momentum vector and kinetic energy post-step, 
location and PDG code. Output in ROOT file. 

## Build instruction

At Warwick, SCRTP, use cvmfs as the easiest environment setup (with bash):

source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc13-opt/setup.sh

Anyone using CentOS7 can source the following environment:

source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-centos7-gcc12-opt/setup.sh

which sets up Geant4 11.2 and GCC13(12) on a CentOS9(7) background. ROOT 6.30 will also 
be available, as is cmake 3.26. Just create a 'build' directory, then 

cd build; cmake ..; make

and run in the build directory for instance as 

./cd109source -m run.mac

Find out about CLI options using --help option.
