#ifndef SipmHit_h
#define SipmHit_h 1

#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4Threading.hh"
#include "G4ThreeVector.hh"
#include "G4VHit.hh"
#include "globals.hh"

#include "G4VPhysicalVolume.hh"

namespace ZDC
{

/// Sipm hit class

class SipmHit : public G4VHit
{
  public:
    SipmHit() = default;
    SipmHit(const SipmHit&) = default;
    ~SipmHit() override = default;

    // operators
    SipmHit& operator=(const SipmHit&) = default;
    G4bool operator==(const SipmHit&) const;

    inline void* operator new(size_t);
    inline void operator delete(void*);

    // methods from base class
    void Draw() override {}
    void Print() override;
    virtual void Clear();

    // get methods: get value from Hit
    G4int GetSipmID() const { return fSipmID; };
    G4double GetHitTime() const { return fHitTime; };

    // set method: set value to Hit
    void SetSipmID(G4int val) { fSipmID = val;  };
    void SetHitTime(G4double val) { fHitTime = val; };
    
  private:
    G4int fSipmID;
    G4double fHitTime;
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using SipmHitsCollection = G4THitsCollection<SipmHit>;

extern G4ThreadLocal G4Allocator<SipmHit>* SipmHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* SipmHit::operator new(size_t)
{
    if (!SipmHitAllocator) {
        SipmHitAllocator = new G4Allocator<SipmHit>;
    }
    void* hit;
    hit = (void*)SipmHitAllocator->MallocSingle();
    return hit;
}

inline void SipmHit::operator delete(void* hit)
{
    if (!SipmHitAllocator) {
        SipmHitAllocator = new G4Allocator<SipmHit>;
    }
    SipmHitAllocator->FreeSingle((SipmHit*)hit);
}

}  // namespace ZDC

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
