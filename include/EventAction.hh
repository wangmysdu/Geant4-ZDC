#ifndef ZDCEventAction_h
#define ZDCEventAction_h 1

#include "G4UserEventAction.hh"

#include "CalorHit.hh"
#include "SiPMHit.hh"
#include "Constants.hh"
#include "globals.hh"
#include "EventMessenger.hh"

class G4Event;

namespace ZDC
{

/// Event action class
///
/// In EndOfEventAction(), it prints the accumulated quantities of the energy
/// deposit and track lengths of charged particles in Absober and Gap layers
/// stored in the hits collections.

class EventAction : public G4UserEventAction
{
    public:
    EventAction();
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction(const G4Event* event) override;

    G4int GetEventID() { return fEventID; }
    void SetEventID(G4int eventID) { fEventID=eventID; }

    // Tracking action function
    void AddEdep_pi0(G4double Edep) { fEPi0 += Edep; }
    void AddEscapeKine(G4double Edep) {  fEscapeKine += Edep; }
    void AddEscape_KineAndNonBaryonMass(G4double Edep) { fEscapeKineAndNonBaryonMass += Edep; }

    // Particle information
    std::vector<G4int>&    GetDetectorID()              { return detectorID; }
    std::vector<G4int>&    GetParticlePDG()             { return particlePDG; }
    std::vector<G4double>& GetParticleEdep()            { return particleEdep; }
    std::vector<G4double>& GetParticleTrackLength()     { return particleTrackLength; }
    std::vector<G4double>& GetParticleInPx()            { return particleInPx; }
    std::vector<G4double>& GetParticleInPy()            { return particleInPy; }
    std::vector<G4double>& GetParticleInPz()            { return particleInPz; }
    std::vector<G4double>& GetParticleOutPx()           { return particleOutPx; }
    std::vector<G4double>& GetParticleOutPy()           { return particleOutPy; }
    std::vector<G4double>& GetParticleOutPz()           { return particleOutPz; }
    std::vector<G4double>& GetParticleInx()             { return particleInx; }
    std::vector<G4double>& GetParticleIny()             { return particleIny; }
    std::vector<G4double>& GetParticleInz()             { return particleInz; }
    std::vector<G4double>& GetParticleOutx()            { return particleOutx; }
    std::vector<G4double>& GetParticleOuty()            { return particleOuty; }
    std::vector<G4double>& GetParticleOutz()            { return particleOutz; }
    std::vector<G4double>& GetParticleTime()            { return particleTime; }

    std::vector<G4double>& GetEEmcModule()              { return EEmcModule; }
    std::vector<G4double>& GetModEdepSen(const int & i) { return ModEdepSen[i]; }
    std::vector<G4double>& GetModEdepAbs(const int & i) { return ModEdepAbs[i]; }

    std::vector<G4int>& GetSipmID()                     { return SipmID; }
    std::vector<G4double>& GetSipmTime()                { return SipmTime; }

    std::vector<G4int>& GetNPEEmcModule()               { return NPEEmcModule; }
    std::vector<G4int>& GetModNPE(const int & i)        { return ModNPE[i]; }

    void SetNewValueInt(G4String key, G4int value);
    
    private:
    // methods
    CalorHitsCollection* GetHitsCollection(G4int hcID, const G4Event* event) const;
    SipmHitsCollection* GetHitsCollection2(G4int hcID, const G4Event* event) const;
    void PrintEventStatistics(G4double absoEdep, G4double absoTrackLength, G4double gapEdep,
                              G4double gapTrackLength) const;

    std::vector<int>    detectorID;
    std::vector<int>    particlePDG;
    std::vector<double> particleEdep;
    std::vector<double> particleTrackLength;
    std::vector<double> particleInPx;
    std::vector<double> particleInPy;
    std::vector<double> particleInPz;
    std::vector<double> particleOutPx;
    std::vector<double> particleOutPy;
    std::vector<double> particleOutPz;
    std::vector<double> particleInx;
    std::vector<double> particleIny;
    std::vector<double> particleInz;
    std::vector<double> particleOutx;
    std::vector<double> particleOuty;
    std::vector<double> particleOutz;
    std::vector<double> particleTime;

    std::vector<G4double> EEmcModule;
    std::vector<G4double> ModEdepSen[3];
    std::vector<G4double> ModEdepAbs[3];

    std::vector<G4int> SipmID;
    std::vector<G4double> SipmTime;

    G4int fNPEEmcTot;
    G4int fSecNEP[NSector];

    std::vector<G4int> NPEEmcModule;
    std::vector<G4int> ModNPE[3];

    G4int fEventID;
    time_t start, end;

    G4double fPbWO4TotalEdep;
    G4double fSecEdepTotSen[NSector];
    G4double fSecEdepTotAbs[NSector];

    // data members
    G4int fAbsHCID = -1;
    G4int fGapHCID = -1;
    G4int fSipmHCID = -1;

    EventMessenger* fMessenger;

    G4int fNEmc = kNEMc;
    G4int fNWSci = kNWSci ;
    G4int fNShash = kNShash;

    G4double fEPi0;
    G4double fEscapeKine;
    G4double fEscapeKineAndNonBaryonMass;


};

}  // namespace ZDC

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
