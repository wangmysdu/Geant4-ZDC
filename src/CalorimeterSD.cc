#include "CalorHit.hh"
#include "CalorimeterSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "RunAction.hh"

namespace ZDC
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorimeterSD::CalorimeterSD(const G4String& name, const G4String& hitsCollectionName)
  : G4VSensitiveDetector(name)
{
    collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
    // Create hits collection
    fHitsCollection = new CalorHitsCollection(SensitiveDetectorName, collectionName[0]);

    // Add this collection in hce
    auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection(hcID, fHitsCollection);

  // Create hits
  // fNofCells for cells + one more for total sums
//  for (G4int i = 0; i < fNofCells + 1; i++) {
//  for (G4int i=0; i<2*(c*c)*kNofLayers*100; i++ ) {
//    fHitsCollection->insert(new CalorHit());
//  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool CalorimeterSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{

    if (!fHitsCollection) return false;
    // energy deposit
    auto edep = step->GetTotalEnergyDeposit()/MeV;

    G4Track *theTrack = step->GetTrack();

    // step length
    G4double stepLength = 0.;
    if (step->GetTrack()->GetDefinition()->GetPDGCharge() != 0.) {
        stepLength = step->GetStepLength()/cm;
    }

    if (edep == 0. && stepLength == 0.) return false;

    G4StepPoint *preStepPoint = step->GetPreStepPoint();
    G4StepPoint *postStepPoint = step->GetPostStepPoint();
    G4TouchableHandle theTouchable = preStepPoint->GetTouchableHandle();
    G4VPhysicalVolume *thePhysVol = theTouchable->GetVolume();

    G4int PID = theTrack->GetParticleDefinition()->GetPDGEncoding();
    G4int TrackID = theTrack->GetTrackID();
    G4int ParentTrackID = theTrack->GetParentID();

    G4String currentPhysicalName = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
    G4String particleName = step->GetTrack()->GetDefinition()->GetParticleName();

    G4int DetectorID = 0;

    for (G4int i = 0; i < theTouchable->GetHistoryDepth(); i++)
        DetectorID += theTouchable->GetCopyNumber(i);

    G4ThreeVector InPos = preStepPoint->GetPosition();
    G4ThreeVector InMom = preStepPoint->GetMomentum();

    G4ThreeVector OutPos = postStepPoint->GetPosition();
    G4ThreeVector OutMom = postStepPoint->GetMomentum();

    G4ThreeVector VertexPos = theTrack->GetVertexPosition();

    G4double InZ = (InPos.z() + OutPos.z()) / 2.0;
    G4double InBeta = preStepPoint->GetBeta();

    G4double Time = preStepPoint->GetGlobalTime();

    G4String theMat = thePhysVol->GetLogicalVolume()->GetMaterial()->GetName();

    G4int CopyNo = theTouchable->GetCopyNumber();

    CalorHit *aHit = NULL;

    for (G4int i = fHitsCollection->entries() - 1; i >= 0; i--) {
        if ((*fHitsCollection)[i]->GetTrackID() == TrackID && (*fHitsCollection)[i]->GetDetectorID() == DetectorID) {
            aHit = (*fHitsCollection)[i];
            break;
        }
    }

    // if found an exits hit, refresh it and accumulate the deposited energy
    // if not, create a new hit and push it into the collection
    if (aHit) {
        aHit->AddEdep(edep);
        aHit->AddTrackLength(stepLength);

        if (aHit->GetTrackID() == TrackID) {
            if (aHit->GetTime() > Time) aHit->SetTime(Time);

            aHit->SetOutPos(OutPos);
            aHit->SetOutMom(OutMom);
        }
    } else {
        // create a new hit
        aHit = new CalorHit();

        aHit->SetPID(PID);
        aHit->SetTrackID(TrackID);
        aHit->SetParentTrackID(ParentTrackID);
        aHit->SetDetectorID(DetectorID);
        aHit->SetInPos(InPos);
        aHit->SetInMom(InMom);
        aHit->SetOutPos(OutPos);
        aHit->SetOutMom(OutMom);
        aHit->SetVertexPos(VertexPos);
        aHit->SetTime(Time);
        aHit->SetEdep(edep);
        aHit->SetTrackLength(stepLength);
        aHit->SetPhysV(thePhysVol);
        aHit->SetCopyNo(CopyNo);
	    aHit->SetPhysVolName(currentPhysicalName);
	    aHit->SetParticleName(particleName);
        fHitsCollection->insert(aHit);
    }
    
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CalorimeterSD::EndOfEvent(G4HCofThisEvent*)
{
    if (verboseLevel > 1) {
        auto nofHits = fHitsCollection->entries();
        G4cout << G4endl << "-------->Hits Collection: in this event they are " << nofHits
               << " hits in the tracker chambers: " << G4endl;
        for (std::size_t i = 0; i < nofHits; ++i)
        (*fHitsCollection)[i]->Print();
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace ZDC
