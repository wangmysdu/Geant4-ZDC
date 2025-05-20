#include "RunAction.hh"
#include "EventAction.hh"
#include "Constants.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "globals.hh"

#include "G4Run.hh"

namespace ZDC
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(EventAction* eventAction)
 : G4UserRunAction(),
   fEventAction(eventAction)	
{
  // set printing event number per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);

  // Create analysis manager
  // The choice of the output format is done via the specified
  // file extension.
  auto analysisManager = G4AnalysisManager::Instance();

  // Create directories
  // analysisManager->SetHistoDirectoryName("histograms");
  // analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
  // Note: merging ntuples is available only with Root output

  // Creating ntuple
  //
  analysisManager->CreateNtuple("ZDC", "Hits");
  analysisManager->CreateNtupleIColumn("EventID");
  
  if (UsePbWO4EMCal) 
  analysisManager->CreateNtupleDColumn("Sec.EdepEMC");
  analysisManager->CreateNtupleDColumn("Sec.EdepSecSen3");
  analysisManager->CreateNtupleDColumn("Sec.EdepSecAbs3");
  analysisManager->CreateNtupleDColumn("Sec.EdepSecSen2");
  analysisManager->CreateNtupleDColumn("Sec.EdepSecAbs2");
  analysisManager->CreateNtupleDColumn("Sec.EdepSecSen1");
  analysisManager->CreateNtupleDColumn("Sec.EdepSecAbs1");

  if (UsePbWO4EMCal) 
  analysisManager->CreateNtupleIColumn("Sec.NPEEmc");
  analysisManager->CreateNtupleIColumn("Sec.NPESec3");
  analysisManager->CreateNtupleIColumn("Sec.NPESec2");
  analysisManager->CreateNtupleIColumn("Sec.NPESec1");

  if (UsePbWO4EMCal) 
  analysisManager->CreateNtupleDColumn("Mod.EdepEmc",       fEventAction->GetEEmcModule());
  analysisManager->CreateNtupleDColumn("Mod.EdepSecAbs3",   fEventAction->GetModEdepAbs(2));
  analysisManager->CreateNtupleDColumn("Mod.EdepSecAbs2",   fEventAction->GetModEdepAbs(1));
  analysisManager->CreateNtupleDColumn("Mod.EdepSecAbs1",   fEventAction->GetModEdepAbs(0));
  analysisManager->CreateNtupleDColumn("Mod.EdepSecSen3",   fEventAction->GetModEdepSen(2));
  analysisManager->CreateNtupleDColumn("Mod.EdepSecSen2",   fEventAction->GetModEdepSen(1));
  analysisManager->CreateNtupleDColumn("Mod.EdepSecSen1",   fEventAction->GetModEdepSen(0));

  if (UsePbWO4EMCal) 
  analysisManager->CreateNtupleIColumn("Mod.NPEEmc",        fEventAction->GetNPEEmcModule());
  analysisManager->CreateNtupleIColumn("Mod.NPESec3",       fEventAction->GetModNPE(2));
  analysisManager->CreateNtupleIColumn("Mod.NPESec2",       fEventAction->GetModNPE(1));
  analysisManager->CreateNtupleIColumn("Mod.NPESec1",       fEventAction->GetModNPE(0));
  
  analysisManager->CreateNtupleIColumn("Sipm.ID",           fEventAction->GetSipmID());
  analysisManager->CreateNtupleDColumn("Sipm.Time",         fEventAction->GetSipmTime());

  analysisManager->CreateNtupleIColumn("Par.DetID",         fEventAction->GetDetectorID());
  analysisManager->CreateNtupleIColumn("Par.PDG",           fEventAction->GetParticlePDG());
  analysisManager->CreateNtupleDColumn("Par.Edep",          fEventAction->GetParticleEdep());
  analysisManager->CreateNtupleDColumn("Par.TrackLength",   fEventAction->GetParticleTrackLength());
  analysisManager->CreateNtupleDColumn("Par.InPx",          fEventAction->GetParticleInPx());
  analysisManager->CreateNtupleDColumn("Par.InPy",          fEventAction->GetParticleInPy());
  analysisManager->CreateNtupleDColumn("Par.InPz",          fEventAction->GetParticleInPz());
  analysisManager->CreateNtupleDColumn("Par.OutPx",         fEventAction->GetParticleOutPx());
  analysisManager->CreateNtupleDColumn("Par.OutPy",         fEventAction->GetParticleOutPy());
  analysisManager->CreateNtupleDColumn("Par.OutPz",         fEventAction->GetParticleOutPz());
  analysisManager->CreateNtupleDColumn("Par.InX",           fEventAction->GetParticleInx());
  analysisManager->CreateNtupleDColumn("Par.InY",           fEventAction->GetParticleIny());
  analysisManager->CreateNtupleDColumn("Par.InZ",           fEventAction->GetParticleInz());
  analysisManager->CreateNtupleDColumn("Par.Outx",          fEventAction->GetParticleOutx());
  analysisManager->CreateNtupleDColumn("Par.Outy",          fEventAction->GetParticleOuty());
  analysisManager->CreateNtupleDColumn("Par.Outz",          fEventAction->GetParticleOutz());
  analysisManager->CreateNtupleDColumn("Par.Time",          fEventAction->GetParticleTime());  
  analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* /*run*/)
{
  // inform the runManager to save random number seed
  // G4RunManager::GetRunManager()->SetRandomNumberStore(true);

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  G4String fileName = "ZDC.root";

  analysisManager->OpenFile(fileName);
  G4cout << "Using " << analysisManager->GetType() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* /*run*/)
{
   G4cout<<"Process complete!"<<G4endl;

  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace ZDC
