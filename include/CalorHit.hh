#ifndef CalorHit_h
#define CalorHit_h 1

#include "G4Allocator.hh"
#include "G4THitsCollection.hh"
#include "G4Threading.hh"
#include "G4ThreeVector.hh"
#include "G4VHit.hh"
#include "globals.hh"

#include "G4VPhysicalVolume.hh"

namespace ZDC
{

/// Calorimeter hit class
///
/// It defines data members to store the the energy deposit and track lengths
/// of charged particles in a selected volume:
/// - fEdep, fTrackLength

class CalorHit : public G4VHit
{
public:
    CalorHit() = default;
    CalorHit(const CalorHit&) = default;
    ~CalorHit() override = default;

    // operators
    CalorHit& operator=(const CalorHit&) = default;
    G4bool operator==(const CalorHit&) const;

    inline void* operator new(size_t);
    inline void operator delete(void*);

    // methods from base class
    void Draw() override {}
    void Print() override;
    virtual void Clear();

    // methods to handle data
    void Add(G4double de, G4double dl);

    // get methods
    G4double GetEdep() const;
    G4double GetTrackLength() const;

    G4int GetPID() const;
    G4int GetTrackID() const;
    G4int GetParentTrackID() const;
    G4int GetDetectorID() const;
    G4ThreeVector GetInPos() const;
    G4ThreeVector GetOutPos() const;
    G4ThreeVector GetVertexPos() const;
    G4ThreeVector GetInMom() const;
    G4ThreeVector GetOutMom() const;
    G4double GetTime() const;
    const G4VPhysicalVolume *GetPhysV() const;
    G4int GetCopyNo() const;
    G4String GetPhysVolName() const;
    G4String GetParticleName() const;

    void SetPID(G4int &val);
    void SetTrackID(G4int &val);
    void SetParentTrackID(G4int &val);
    void SetDetectorID(G4int &val);
    void SetInPos(G4ThreeVector &xyz);
    void SetOutPos(G4ThreeVector &xyz);
    void SetVertexPos(G4ThreeVector &xyz);
    void SetInMom(G4ThreeVector &pxpypz);
    void SetOutMom(G4ThreeVector &pxpypz);
    void SetTime(G4double &val);
    void SetEdep(G4double &val);
    void SetTrackLength(G4double &val);
    void SetPhysV(G4VPhysicalVolume *val);
    void SetCopyNo(G4int &val);

    void SetPhysVolName(G4String &val);
    void SetParticleName(G4String &val);

    void AddEdep(G4double &val);
    void AddTrackLength(G4double &val);
    
private:
    G4double fEdep = 0.;  ///< Energy deposit in the sensitive volume
    G4double fTrackLength = 0.;  ///< Track length in the  sensitive volume

    G4int         fPID;
    G4int         fTrackID;
    G4int         fPTrackID;
    G4int         fDetID;
    G4ThreeVector fInPos;
    G4ThreeVector fOutPos;
    G4ThreeVector fVertexPos;
    G4ThreeVector fInMom;
    G4ThreeVector fOutMom;
    G4double      fTime;
    const G4VPhysicalVolume *fPhysV;
    G4int         fCopyNo;
    G4String      fPhysicalVolName;
    G4String      fParticleName;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using CalorHitsCollection = G4THitsCollection<CalorHit>;

extern G4ThreadLocal G4Allocator<CalorHit>* CalorHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* CalorHit::operator new(size_t)
{
    if (!CalorHitAllocator) {
        CalorHitAllocator = new G4Allocator<CalorHit>;
    }
    void* hit;
    hit = (void*)CalorHitAllocator->MallocSingle();
    return hit;
}

inline void CalorHit::operator delete(void* hit)
{
    if (!CalorHitAllocator) {
        CalorHitAllocator = new G4Allocator<CalorHit>;
    }
    CalorHitAllocator->FreeSingle((CalorHit*)hit);
}

inline void CalorHit::Add(G4double de, G4double dl)
{
    fEdep += de;
    fTrackLength += dl;
}

inline G4double CalorHit::GetEdep() const
{
    return fEdep;
}

inline G4double CalorHit::GetTrackLength() const
{
    return fTrackLength;
}

inline G4int CalorHit::GetPID() const
{
    return fPID;
}

inline G4int CalorHit::GetTrackID() const
{
    return fTrackID;
}

inline G4int CalorHit::GetParentTrackID() const
{
    return fPTrackID;
}

inline G4int CalorHit::GetDetectorID() const
{
    return fDetID;
}

inline G4ThreeVector CalorHit::GetInPos() const
{
    return fInPos;
}

inline G4ThreeVector CalorHit::GetOutPos() const
{
    return fOutPos;
}

inline G4ThreeVector CalorHit::GetVertexPos() const
{
    return fVertexPos;
}

inline G4ThreeVector CalorHit::GetInMom() const
{
    return fInMom;
}

inline G4ThreeVector CalorHit::GetOutMom() const
{
    return fOutMom;
}


inline G4double CalorHit::GetTime() const
{
    return fTime;
}

inline const G4VPhysicalVolume *CalorHit::GetPhysV() const
{
    return fPhysV;
}

inline G4int CalorHit::GetCopyNo() const
{
    return fCopyNo;
}

inline G4String CalorHit::GetPhysVolName() const
{
    return fPhysicalVolName;
}

inline G4String CalorHit::GetParticleName() const
{
    return fParticleName;
}


inline void CalorHit::SetPID(G4int &val)
{
    fPID = val;
}

inline void CalorHit::SetTrackID(G4int &val)
{
    fTrackID = val;
}

inline void CalorHit::SetParentTrackID(G4int &val)
{
    fPTrackID = val;
}

inline void CalorHit::SetDetectorID(G4int &val)
{
    fDetID = val;
}

inline void CalorHit::SetInPos(G4ThreeVector &xyz)
{
    fInPos = xyz;
}

inline void CalorHit::SetOutPos(G4ThreeVector &xyz)
{
    fOutPos = xyz;
}

inline void CalorHit::SetVertexPos(G4ThreeVector &xyz)
{
    fVertexPos = xyz;
}

inline void CalorHit::SetInMom(G4ThreeVector &pxpypz)
{
    fInMom = pxpypz;
}

inline void CalorHit::SetOutMom(G4ThreeVector &pxpypz)
{
    fOutMom = pxpypz;
}

inline void CalorHit::SetTime(G4double &val)
{
    fTime = val;
}

inline void CalorHit::SetEdep(G4double &val)
{
    fEdep = val;
}

inline void CalorHit::SetTrackLength(G4double &val)
{
    fTrackLength = val;
}

inline void CalorHit::SetPhysV(G4VPhysicalVolume *val)
{
    fPhysV = val;
}

inline void CalorHit::SetCopyNo(G4int &val)
{
    fCopyNo = val;
}

inline void CalorHit::SetPhysVolName(G4String &val)
{
    fPhysicalVolName = val;
}

inline void CalorHit::SetParticleName(G4String &val)
{
    fParticleName = val;
}

inline void CalorHit::AddEdep(G4double &val)
{
    fEdep += val;
}

inline void CalorHit::AddTrackLength(G4double &val)
{
    fTrackLength += val;
}


}  // namespace ZDC

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
