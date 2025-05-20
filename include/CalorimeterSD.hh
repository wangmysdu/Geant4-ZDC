#ifndef ZDCCalorimeterSD_h
#define ZDCCalorimeterSD_h 1

#include "Constants.hh"

#include "CalorHit.hh"

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

namespace ZDC
{

/// Calorimeter sensitive detector class
///
/// In Initialize(), it creates one hit for each calorimeter layer and one more
/// hit for accounting the total quantities in all layers.
///
/// The values are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step.

class CalorimeterSD : public G4VSensitiveDetector
{
public:
    CalorimeterSD(const G4String& name, const G4String& hitsCollectionName);
    ~CalorimeterSD() override = default;

    // methods from base class
    void Initialize(G4HCofThisEvent* hitCollection) override;
    G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    void EndOfEvent(G4HCofThisEvent* hitCollection) override;

private:
    CalorHitsCollection* fHitsCollection = nullptr;
    G4int fNofCells = 0;
};

}  // namespace ZDC

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
