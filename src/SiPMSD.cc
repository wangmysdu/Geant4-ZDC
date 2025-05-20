#include "SiPMHit.hh"
#include "SiPMSD.hh"

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

SipmSD::SipmSD(const G4String& name, const G4String& hitsCollectionName)
  : G4VSensitiveDetector(name)
{
    collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SipmSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection
  fHitsCollection = new SipmHitsCollection(SensitiveDetectorName, collectionName[0]); //SensitiveDetectorName 是啥？没有定义也没有用到

  // Add this collection in hce
  auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection(hcID, fHitsCollection);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool SipmSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    G4String currentPhysicalName = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
	G4String particleName = step->GetTrack()->GetDefinition()->GetParticleName();

  //if(currentPhysicalName=="SipmPhysical") { //应该都是，需确认 PMT or Sipm
	if(particleName == "opticalphoton") {
        SipmHit *aHit = new SipmHit();

		G4Track* track = step->GetTrack();
		G4StepPoint* prePoint = step->GetPreStepPoint();
      // Sipm hit
        if(currentPhysicalName == "PMTPhysical_EM") { //EM
            aHit->SetSipmID(prePoint->GetTouchable()->GetCopyNumber(1)); //Sipm -> Crystal
        }
        else { //Shahslik
            aHit->SetSipmID(prePoint->GetTouchable()->GetCopyNumber(2) + prePoint->GetTouchable()->GetCopyNumber(1)); //Sipm -> 16 holes -> Shashlik
        }
		//time
		aHit->SetHitTime(prePoint->GetGlobalTime());

		//stop and kill photons
		track->SetTrackStatus(fStopAndKill); //确认能不能对track操作

        fHitsCollection->insert(aHit);
	}
	//}
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SipmSD::EndOfEvent(G4HCofThisEvent*)
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace ZDC
