# Photo-electron source Example

This example Geant4 application models the action of a hypothetical photo-electron source 
for the QTNM project, simulating an x-ray source aiming at a metal plate.

## Notes 

Default physics list is EMStandard_opt4, high precision low-energy EM physics.
Uses the GPS particle source for flexible event generation.

Geometry is a vacuum sphere containing a scoring sphere and a cylindrical metal plate. The source
is the general particle source which enables a lot of flexibility to configure it. The scoring
sphere has a radius of 5 cm, the plate is a 2.5 cm diameter, 5 mm thick cylinder, thick enough
to stop most x-ray energies for mid-to-heavy elemental metal materials (Al is a bit too low in Z
so that x-rays still pass but not most). There is a predefinition of metals to choose from, currently
it is a nickel plate. A single code line in PEDetectorConstruction.cc needs to be changed to change
that target metal (with subsequent re-compilation). This was the simplest choice on varying the 
material, avoiding a user macro command
and new construction of the geometry before the run. Can be changed but for the purpose of 
experimentation, this seemed far easier to implement.

A scoring surface, a sphere around the source, is defined as a 
boundary between two vacuum spaces, Scorer and World, purely for scoring of a free, emitted electron.
Condition for scoring is that a step is at a geometry boundary and between identical materials (vacuum).

Scorer: scoring surface crossing - particle momentum vector and kinetic energy post-step, 
location and PDG code. Output in ROOT file. A converter script in Python converts the ROOT file 
into a compressed CSV file which can be read back in Python using, for instance NumPy np.loadtxt()
with the delimiter keyword ','. The header is skipped automatically as a comment. This would 
produce a 2D NumPy array with the columns as in the converter script (or see header) and each row 
a scored hit in the simulation. Note that due to the parallel processing, the order of entries
is random hence the event ID numbers in the file to label each event.

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
