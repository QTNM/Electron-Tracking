// ********************************************************************
// scattering example

// standard
#include <algorithm>
#include <string>

// Geant4
#include "G4Types.hh"

#ifdef G4MULTITHREADED
#  include "G4MTRunManager.hh"
#else
#  include "G4RunManager.hh"
#endif

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4Threading.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"

// us
#include "CLI11.hpp"  // c++17 safe; https://github.com/CLIUtils/CLI11
#include "SEActionInitialization.hh"
#include "SEDetectorConstruction.hh"

int main(int argc, char** argv)
{
  // command line interface
  CLI::App    app{ "Scattering example for QTNM" };
  int         nthreads = 4;
  std::string outputFileName("qtnm.root");
  std::string macroName;
  std::string physListName;

  app.add_option("-m,--macro", macroName, "<Geant4 macro filename> Default: None");
  app.add_option("-p,--physlist", physListName, "<Geant4 physics list name> Default: G4EmStandard_opt4");
  app.add_option("-o,--outputFile", outputFileName,
                 "<FULL PATH ROOT FILENAME> Default: lg.root");
  app.add_option("-t, --nthreads", nthreads, "<number of threads to use> Default: 4");

  CLI11_PARSE(app, argc, argv);

  // GEANT4 code
  // Don't accept interactive mode (no macroName).

  if(macroName.empty())
  {
    G4cout << "No interactive mode running of example: provide a macro!" << G4endl;
    return 1;
  }

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
  auto* detector = new SEDetectorConstruction;
  runManager->SetUserInitialization(detector);


  // -- set user physics list
  // Physics list factory
  G4PhysListFactory factory;
  G4VModularPhysicsList* physList = nullptr;

  // Check if the name is known to the factory
  if ( physListName.size() &&  (! factory.IsReferencePhysList(physListName) ) ) {
    G4cerr << "Physics list " << physListName
           << " is not available in PhysListFactory." << G4endl;
    physListName.clear();
  }

  // If name is not defined use recommended low-energy EM physics list
  if ( physListName.empty() ) {
    physListName = "G4EmStandard_opt4";
  }

  // Reference PhysicsList via its name
  physList = factory.GetReferencePhysList(physListName);

  // finish physics list
  runManager->SetUserInitialization(physList);


  // -- Set user action initialization class.
  auto* actions = new SEActionInitialization(outputFileName);
  runManager->SetUserInitialization(actions);

  // Get the pointer to the User Interface manager
  //
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Batch mode only - no visualisation
  G4String command = "/control/execute ";
  UImanager->ApplyCommand(command + macroName);
  
  delete runManager;
  return 0;
}
