#include "BIRunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

BIRunAction::BIRunAction(G4String name)
: G4UserRunAction()
, fout(std::move(name))
{
  // Create analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Create directories
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);

  // Creating ntuple with vector entries
  //
  analysisManager->CreateNtuple("Score", "Hits");
  analysisManager->CreateNtupleIColumn("EventID");
  analysisManager->CreateNtupleIColumn("TrackID");
  analysisManager->CreateNtupleDColumn("Edep");
  analysisManager->CreateNtupleDColumn("Kine");
  analysisManager->CreateNtupleDColumn("Px");
  analysisManager->CreateNtupleDColumn("Py");
  analysisManager->CreateNtupleDColumn("Pz");
  analysisManager->CreateNtupleDColumn("Posx");
  analysisManager->CreateNtupleDColumn("Posy");
  analysisManager->CreateNtupleDColumn("Posz");
  analysisManager->FinishNtuple();

  // Stop Watch
  analysisManager->CreateNtuple("Watch", "Timing");
  analysisManager->CreateNtupleIColumn("EventID");
  analysisManager->CreateNtupleIColumn("ExitID");
  analysisManager->CreateNtupleDColumn("ExitTime");
  analysisManager->CreateNtupleDColumn("Posx");
  analysisManager->CreateNtupleDColumn("Posy");
  analysisManager->CreateNtupleDColumn("Posz");
  analysisManager->CreateNtupleDColumn("Px");
  analysisManager->CreateNtupleDColumn("Py");
  analysisManager->CreateNtupleDColumn("Pz");
  analysisManager->CreateNtupleDColumn("Kine");
  analysisManager->FinishNtuple();

  // Initial Conditions
  analysisManager->CreateNtuple("Setup", "Initial Conditions");
  analysisManager->CreateNtupleIColumn("EventID");
  analysisManager->CreateNtupleIColumn("ExitID");
  analysisManager->CreateNtupleDColumn("EntryTime");
  analysisManager->CreateNtupleDColumn("Posx");
  analysisManager->CreateNtupleDColumn("Posy");
  analysisManager->CreateNtupleDColumn("Posz");
  analysisManager->CreateNtupleDColumn("Px");
  analysisManager->CreateNtupleDColumn("Py");
  analysisManager->CreateNtupleDColumn("Pz");
  analysisManager->CreateNtupleDColumn("Kine");
  analysisManager->FinishNtuple();
}

BIRunAction::~BIRunAction() { delete G4AnalysisManager::Instance(); }

void BIRunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  analysisManager->OpenFile(fout);
}

void BIRunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // save ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}
