#include "TrackingAction.hh"
#include "EventAction.hh"

#include "G4Track.hh"
#include "G4TrackingManager.hh"

namespace ZDC
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
TrackingAction::TrackingAction(EventAction* eventAction) : G4UserTrackingAction(),fEventAction(eventAction)
{
}

void TrackingAction::PreUserTrackingAction(const G4Track* aTrack)
{
    if(aTrack->GetDefinition()->GetParticleName()=="pi0"){
        double edep = aTrack->GetTotalEnergy();
        fEventAction->AddEdep_pi0(edep);
    }
    // if(aTrack->GetDefinition()->GetParticleName()=="mu-" || aTrack->GetDefinition()->GetParticleName()=="mu+"){
    //     G4cout<<"muon: "<<aTrack->GetTotalEnergy()<<G4endl;
    // }
}

// end of particle
void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{
    const G4ParticleDefinition* particle = aTrack->GetParticleDefinition();

    // 判断是否从世界边界逃逸
    const G4StepPoint* postStepPoint = aTrack->GetStep()->GetPostStepPoint();
    if (postStepPoint && postStepPoint->GetStepStatus() == fWorldBoundary) {
        fEventAction->AddEscapeKine(aTrack->GetKineticEnergy());
        fEventAction->AddEscape_KineAndNonBaryonMass(aTrack->GetKineticEnergy());
        // G4cout << "[Escape] "
        //         << particle->GetParticleName()
        //         << ", E = " << aTrack->GetKineticEnergy() / MeV << " MeV"
        //         //<< ", TrackID = " << aTrack->GetTrackID()
        //         << G4endl;

        // 判断是否是非重子，是的话总丢失能量加入质量
        if (particle->GetBaryonNumber() == 0) {
            fEventAction->AddEscape_KineAndNonBaryonMass(aTrack->GetParticleDefinition()->GetPDGMass());
        }
    }
}
         
} // namespace ZDC
