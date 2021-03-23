#include "SERunAction.hh"
#include "SEEventAction.hh"
#include "g4root.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

SERunAction::SERunAction(SEEventAction* eventAction, G4String name)
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
  analysisManager->CreateNtupleIColumn("HitID");
//  analysisManager->CreateNtupleIColumn("HitID", fEventAction->GetHitID());
  analysisManager->CreateNtupleIColumn("ParentID");
//  analysisManager->CreateNtupleIColumn("ParentID", fEventAction->GetParentID());
  analysisManager->CreateNtupleDColumn("KinE");
//  analysisManager->CreateNtupleDColumn("KinE", fEventAction->GetHitKinE());
  analysisManager->CreateNtupleDColumn("Time");
//  analysisManager->CreateNtupleDColumn("Time", fEventAction->GetHitTime());
  analysisManager->CreateNtupleIColumn("ExitID");
//  analysisManager->CreateNtupleIColumn("ExitID", fEventAction->GetExitID());
  analysisManager->CreateNtupleDColumn("ExitTime");
//  analysisManager->CreateNtupleDColumn("ExitTime", fEventAction->GetExitTime());

  analysisManager->FinishNtuple();
}

SERunAction::~SERunAction() { delete G4AnalysisManager::Instance(); }

void SERunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  analysisManager->OpenFile(fout);
}

void SERunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // save ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}
