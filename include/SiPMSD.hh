#ifndef ZDCSipmSD_h
#define ZDCSipmSD_h 1

#include "Constants.hh"

#include "SiPMHit.hh"

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

class G4Step;
class G4HCofThisEvent;

class G4TouchableHistory;

namespace ZDC
{

class SipmSD : public G4VSensitiveDetector
{
  public:
    SipmSD(const G4String& name, const G4String& hitsCollectionName);
    ~SipmSD() override = default;

    // methods from base class
    void Initialize(G4HCofThisEvent* hitCollection) override;
    G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    void EndOfEvent(G4HCofThisEvent* hitCollection) override;

  private:
    SipmHitsCollection* fHitsCollection = nullptr;
};

}  // namespace ZDC

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
