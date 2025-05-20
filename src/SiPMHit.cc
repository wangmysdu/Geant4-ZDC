#include "SiPMHit.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "G4ios.hh"

#include <iomanip>

namespace ZDC
{

G4ThreadLocal G4Allocator<SipmHit>* SipmHitAllocator = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool SipmHit::operator==(const SipmHit& right) const
{
    return (this == &right) ? true : false;
}

// G4bool SipmHit::operator ==(const SipmHit &right) const
// {
//     return ((fPID == right.fPID) && (fTrackID == right.fTrackID) && (fDetID == right.fDetID) && (fPhysV == right.fPhysV) && (fCopyNo == right.fCopyNo));
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SipmHit::Print()
{
  
}

void SipmHit::Clear()
{
    fSipmID=0;
    fHitTime=0;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace ZDC
