# Physics Validation Test Example

This example Geant4 application addresses validation of various Physics Lists. It follows 
the TestEm13 Geant4 example in examples/extended/electromagnetic/TestEm13.

## Notes 

Available physics lists are QTNMPhysicsList, G4EmStandardPhysicsSS for a G4 single scattering example physics list and a 
Standard EM list from G4.

Change of physics list, some target materials and primary particle parameters is possible by macro.

## Build instruction

At Warwick, SCRTP, use cvmfs as the easiest environment setup (with bash):

source /cvmfs/sft.cern.ch/lcg/views/LCG_105/x86_64-el9-gcc13-opt/setup.sh

cd build; cmake ..; make

and run in the build directory (copy run.mac into it)

./TestEm13 run.mac
