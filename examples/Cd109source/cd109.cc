// ********************************************************************
// Cd-109 source example

// standard
#include <algorithm>
#include <string>
#include <vector>

// Geant4
#include "G4Types.hh"
#include "Randomize.hh"

#ifdef G4MULTITHREADED
#  include "G4MTRunManager.hh"
#else
#  include "G4RunManager.hh"
#endif

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4Threading.hh"
#include "G4GenericPhysicsList.hh"
#include "G4VModularPhysicsList.hh"

// us
#include "CLI11.hpp"  // c++17 safe; https://github.com/CLIUtils/CLI11
#include "CDActionInitialization.hh"
#include "CDDetectorConstruction.hh"

int main(int argc, char** argv)
{
  // command line interface
  CLI::App    app{ "Electron source for QTNM" };
  int         nthreads = 4;
  int         seed     = 1234;
  std::string outputFileName("cd109.root");
  std::string macroName;

  app.add_option("-m,--macro", macroName, "<Geant4 macro filename> Default: None");
  app.add_option("-s,--seed", seed, "<Geant4 random number seed + offset 1234> Default: 1234");
  app.add_option("-o,--outputFile", outputFileName,
                 "<FULL PATH ROOT FILENAME> Default: cd109.root");
  app.add_option("-t, --nthreads", nthreads, "<number of threads to use> Default: 4");

  CLI11_PARSE(app, argc, argv);

  // GEANT4 code
  // Get the pointer to the User Interface manager
  //
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Don't accept interactive mode (no macroName).
  if(macroName.empty())
  {
    G4cout << "No interactive mode running of example: provide a macro!" << G4endl;
    return 1;
  }

  // set the random seed + offset 1234; avoiding zero seed -> runtime error
  CLHEP::HepRandom::setTheSeed(1234+seed);

  // -- Construct the run manager : MT or sequential one
  auto* runManager = G4RunManagerFactory::CreateRunManager();
#ifdef G4MULTITHREADED
  nthreads =
    std::min(nthreads, G4Threading::G4GetNumberOfCores());  // limit thread number to
                                                            // max on machine
  G4cout << "      ********* Run Manager constructed in MT mode: " << nthreads
         << " threads ***** " << G4endl;
  runManager->SetNumberOfThreads(nthreads);
#endif


  // -- Set mandatory initialization classes
  auto* detector = new CDDetectorConstruction;
  runManager->SetUserInitialization(detector);


  // -- set user physics list
  // Physics list factory
  G4VModularPhysicsList* physList = nullptr;

  // from vector of physics constructor names
  std::vector<G4String>* myConstructors = new std::vector<G4String>;
  myConstructors->push_back("G4EmStandardPhysics_option4");
  myConstructors->push_back("G4RadioactiveDecayPhysics");
  physList = new G4GenericPhysicsList(myConstructors);

  // finish physics list
  runManager->SetUserInitialization(physList);


  // -- Set user action initialization class.
  auto* actions = new CDActionInitialization(outputFileName);
  runManager->SetUserInitialization(actions);


  // Batch mode only - no visualisation
  G4String command = "/control/execute ";
  UImanager->ApplyCommand(command + macroName);
  
  delete runManager;
  return 0;
}
