#include "EventAction.hh"

#include "CalorimeterSD.hh"
#include "CalorHit.hh"
#include "SiPMSD.hh"
#include "SiPMHit.hh"
#include "Constants.hh"

#include "G4AnalysisManager.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"

#include <iomanip>
#include "Randomize.hh"

using std::array;
using std::vector;

namespace ZDC
{

EventAction::EventAction()
{
	fMessenger = new EventMessenger(this);
}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorHitsCollection *EventAction::GetHitsCollection(G4int hcID, const G4Event *event) const
{
    auto hitsCollection = static_cast<CalorHitsCollection *>(event->GetHCofThisEvent()->GetHC(hcID));

    if (!hitsCollection)
    {
      G4ExceptionDescription msg;
      msg << "Cannot access hitsCollection ID " << hcID;
      G4Exception("EventAction::GetHitsCollection()", "MyCode0003", FatalException, msg);
    }

    return hitsCollection;
}

SipmHitsCollection *EventAction::GetHitsCollection2(G4int hcID, const G4Event *event) const
{
    auto hitsCollection = static_cast<SipmHitsCollection *>(event->GetHCofThisEvent()->GetHC(hcID));

    if (!hitsCollection)
    {
      G4ExceptionDescription msg;
      msg << "Cannot access hitsCollection ID " << hcID;
      G4Exception("EventAction::GetHitsCollection2()", "MyCode0003", FatalException, msg);
    }

    return hitsCollection;
}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::PrintEventStatistics(G4double absoEdep, G4double absoTrackLength,
                                     G4double gapEdep, G4double gapTrackLength) const
{
    // print event statistics
    G4cout << "   Absorber: total energy: " << std::setw(7) << G4BestUnit(absoEdep, "Energy")
           << "       total track length: " << std::setw(7) << G4BestUnit(absoTrackLength, "Length")
           << G4endl << "        Gap: total energy: " << std::setw(7) << G4BestUnit(gapEdep, "Energy")
           << "       total track length: " << std::setw(7) << G4BestUnit(gapTrackLength, "Length")
           << G4endl;
}

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event * /*event*/){
    
    //for PbWO4
    fPbWO4TotalEdep = 0.;
    fNPEEmcTot = 0;
    EEmcModule.clear();
    EEmcModule.resize(fNEmc * fNEmc, 0.);
    NPEEmcModule.clear();
    NPEEmcModule.resize(fNEmc * fNEmc, 0.);
    
    //for the 3 main sectors
    int ArrSize[NSector] = {fNShash * fNShash, fNShash * fNShash, fNWSci * fNWSci};
    for (int i=0; i<NSector; i++){
        fSecEdepTotSen[i] = 0.;
        fSecEdepTotAbs[i] = 0.;
        fSecNEP[i] = 0;
        
        ModEdepSen[i].clear();
        ModEdepAbs[i].clear();
        ModNPE[i].clear();
        
        ModEdepSen[i].resize(ArrSize[i], 0.);
        ModEdepAbs[i].resize(ArrSize[i], 0.);
        ModNPE[i].resize(ArrSize[i], 0);
    }

    SipmID.clear();
    SipmTime.clear();

    detectorID.clear();
    particlePDG.clear();
    particleEdep.clear();
    particleTrackLength.clear();
    particleInPx.clear();
    particleInPy.clear();
    particleInPz.clear();
    particleOutPx.clear();
    particleOutPy.clear();
    particleOutPz.clear();
    particleInx.clear();
    particleIny.clear();
    particleInz.clear();
    particleOutx.clear();
    particleOuty.clear();
    particleOutz.clear();
    particleTime.clear();
    time(&start);
    end = time(NULL);

    fEPi0 = 0;
    fEscapeKine = 0;
    fEscapeKineAndNonBaryonMass = 0;
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event *event)
{
    // Get hits collections IDs (only once)
    if (fAbsHCID == -1){
        fAbsHCID = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection1");
    }

    if (fSipmHCID == -1){
        fSipmHCID = G4SDManager::GetSDMpointer()->GetCollectionID("SipmHitsCollection2");
    }

    fEventID = event->GetEventID();
    // Get hits collections
    auto absoHC = GetHitsCollection(fAbsHCID, event);
    auto SipmHC = GetHitsCollection2(fSipmHCID, event);

    // Get hit with total values
    //auto absoHit = (*absoHC)[absoHC->entries() - 1];

    // Print per event (modulo n)
    auto eventID = event->GetEventID();
    auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
    //  if ((printModulo > 0) && (eventID % printModulo == 0)) {
    //    PrintEventStatistics(absoHit->GetEdep(), absoHit->GetTrackLength(), gapHit->GetEdep(),
    //                         gapHit->GetTrackLength());
    G4cout << "--> End of event: " << eventID << "\n"
           << G4endl;

    // get analysis manager
    auto analysisManager = G4AnalysisManager::Instance();
    for (G4int i = absoHC->entries() - 1; i >= 0; i--){
        CalorHit *aHit = (*absoHC)[i];

        if (aHit->GetEdep() < 0.0001) continue;

        
        if (aHit->GetPhysVolName() == "LeadPhysical_1"){
            fSecEdepTotAbs[0] += aHit->GetEdep();
            int NModule = (aHit->GetDetectorID()- 1000000)/10000 -1;
            ModEdepAbs[0][NModule] += aHit->GetEdep();
        }
        else if (aHit->GetPhysVolName() == "ScinPhysical_1"){
            fSecEdepTotSen[0] += aHit->GetEdep();
            int NModule = (aHit->GetDetectorID()- 1000000)/10000 -1;
            ModEdepSen[0][NModule] += aHit->GetEdep();
        }
        else if (aHit->GetPhysVolName() == "LeadPhysical_2"){
            fSecEdepTotAbs[1] += aHit->GetEdep();
            int NModule = (aHit->GetDetectorID()- 2000000)/10000 -1;
            ModEdepAbs[1][NModule] += aHit->GetEdep();
        }
        else if (aHit->GetPhysVolName() == "ScinPhysical_2"){
            fSecEdepTotSen[1] += aHit->GetEdep();
            int NModule = (aHit->GetDetectorID()- 2000000)/10000 -1;
            ModEdepSen[1][NModule] += aHit->GetEdep();
        }
        else if (aHit->GetPhysVolName() == "WPhysical"){
            fSecEdepTotAbs[2] += aHit->GetEdep();
            int NModule = (aHit->GetDetectorID()- 3000000)/10000 -1;
            ModEdepAbs[2][NModule] += aHit->GetEdep();
        }
        else if (aHit->GetPhysVolName() == "WScinPhysical"){
            fSecEdepTotSen[2] += aHit->GetEdep();
            int NModule = (aHit->GetDetectorID()- 3000000)/10000 -1;
            ModEdepSen[2][NModule] += aHit->GetEdep();
        }
        else if (aHit->GetPhysVolName() == "CrysPhysical_EM"){
            fPbWO4TotalEdep += aHit->GetEdep();
            int NModule = (aHit->GetDetectorID()- 4000000)/10000 -1;
            EEmcModule[NModule] += aHit->GetEdep();
        }

        //detectorID.push_back(aHit->GetDetectorID());
        //particlePDG.push_back(aHit->GetPID());
        //particleEdep.push_back(aHit->GetEdep());
        //particleTrackLength.push_back(aHit->GetTrackLength());
        //particleInPx.push_back(aHit->GetInMom().x());
        //particleInPy.push_back(aHit->GetInMom().y());
        //particleInPz.push_back(aHit->GetInMom().z());
        //particleOutPx.push_back(aHit->GetOutMom().x());
        //particleOutPy.push_back(aHit->GetOutMom().y());
        //particleOutPz.push_back(aHit->GetOutMom().z());
        //particleInx.push_back(aHit->GetInPos().x());
        //particleIny.push_back(aHit->GetInPos().y());
        //particleInz.push_back(aHit->GetInPos().z());
        //particleOutx.push_back(aHit->GetOutPos().x());
        //particleOuty.push_back(aHit->GetOutPos().y());
        //particleOutz.push_back(aHit->GetOutPos().z());
        //particleTime.push_back(aHit->GetTime());
        
    }

    for (G4int i = SipmHC->entries() - 1; i >= 0; i--){
        SipmHit *aHit = (*SipmHC)[i];

        G4int ID = aHit->GetSipmID();
        G4int IDmodule = (ID /10000) %100 -1;

        SipmID.push_back(ID);
        SipmTime.push_back(aHit->GetHitTime());

        if(ID >= 4000000){  //Emc
            fNPEEmcTot++;
            NPEEmcModule[IDmodule]++;
        }
        else if(ID >= 3000000){  //WSci
            fSecNEP[2]++;
            ModNPE[2][IDmodule]++;
        }
        else if(ID >= 2000000){  //Shashlik 2
            fSecNEP[1]++;
            ModNPE[1][IDmodule]++;
        }
        else if(ID >= 1000000){  //Shashlik 1
            fSecNEP[0]++;
            ModNPE[0][IDmodule]++;
        }
    }

    // fill ntuple
    G4int BranchIdx = 0;
    analysisManager->FillNtupleIColumn(BranchIdx++, fEventID);
    analysisManager->FillNtupleDColumn(BranchIdx++, fEPi0);
    analysisManager->FillNtupleDColumn(BranchIdx++, fEscapeKine);
    analysisManager->FillNtupleDColumn(BranchIdx++, fEscapeKineAndNonBaryonMass);

    if (UsePbWO4EMCal) analysisManager->FillNtupleDColumn(BranchIdx++, fPbWO4TotalEdep);
    analysisManager->FillNtupleDColumn(BranchIdx++, fSecEdepTotSen[2]);
    analysisManager->FillNtupleDColumn(BranchIdx++, fSecEdepTotAbs[2]);
    analysisManager->FillNtupleDColumn(BranchIdx++, fSecEdepTotSen[1]);
    analysisManager->FillNtupleDColumn(BranchIdx++, fSecEdepTotAbs[1]);
    analysisManager->FillNtupleDColumn(BranchIdx++, fSecEdepTotSen[0]);
    analysisManager->FillNtupleDColumn(BranchIdx++, fSecEdepTotAbs[0]);

    if (UsePbWO4EMCal) analysisManager->FillNtupleIColumn(BranchIdx++, fNPEEmcTot);
    analysisManager->FillNtupleIColumn(BranchIdx++, fSecNEP[2]);
    analysisManager->FillNtupleIColumn(BranchIdx++, fSecNEP[1]);
    analysisManager->FillNtupleIColumn(BranchIdx++, fSecNEP[0]);
    
    analysisManager->AddNtupleRow();
  }

  //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


    void EventAction::SetNewValueInt(G4String key, G4int value){
        if(key == "NEmc"){
            fNEmc = value;
            G4cout << "Set NEmc value in EventAction = " << value << G4endl;
        }
        if(key == "NWSci"){
            fNWSci = value;
            G4cout << "Set NWSci value in EventAction = " << value << G4endl;
        }
        if(key == "NShash"){
            fNShash = value;
            G4cout << "Set NShash value in EventAction = " << value << G4endl;
        }
    }

} // namespace ZDC
