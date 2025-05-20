#include "CalorHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "G4Allocator.hh"
#include "G4VHit.hh"
#include "G4ios.hh"

#include <iomanip>

namespace ZDC
{

G4ThreadLocal G4Allocator<CalorHit>* CalorHitAllocator = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//G4bool CalorHit::operator==(const CalorHit& right) const
//{
//  return (this == &right) ? true : false;
//}

G4bool CalorHit::operator ==(const CalorHit &right) const
{
    return ((fPID == right.fPID) && (fTrackID == right.fTrackID) && (fDetID == right.fDetID) && (fPhysV == right.fPhysV) && (fCopyNo == right.fCopyNo));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CalorHit::Print()
{
    G4cout << "Edep: " << std::setw(7) << G4BestUnit(fEdep, "Energy")
           << " track length: " << std::setw(7) << G4BestUnit(fTrackLength, "Length") << G4endl;
}

void CalorHit::Clear()
{
    fPID = 0;
    fTrackID = 0;
    fPTrackID = 0;
    fDetID = 0;
    fInPos.set(0, 0, 0);
    fOutPos.set(0, 0, 0);
    fInMom.set(0, 0, 0);
    fOutMom.set(0, 0, 0);
    fVertexPos.set(0, 0, 0);
    fTime = 0;
    fEdep = 0;
    fTrackLength = 0;
    fPhysV = NULL;
    fCopyNo = 0;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}  // namespace ZDC
