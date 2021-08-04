#include "EGRunAction.hh"
#include "EGEventAction.hh"
#include "g4root.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

EGRunAction::EGRunAction(EGEventAction* eventAction, G4String name)
: G4UserRunAction()
, fEventAction(eventAction)
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
  analysisManager->FinishNtuple();

}

EGRunAction::~EGRunAction() { delete G4AnalysisManager::Instance(); }

void EGRunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  analysisManager->OpenFile(fout);
}

void EGRunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // save ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}
