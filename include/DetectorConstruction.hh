#ifndef ZDCDetectorConstruction_h
#define ZDCDetectorConstruction_h 1

#include "Constants.hh"

#include "G4VUserDetectorConstruction.hh"

#include "ZDCMaterials.hh"
#include "G4Threading.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;

class G4Material;
//class ZDCMaterials;

namespace ZDC
{

class DetectorMessenger;

/// Detector construction class to define materials and geometry.
/// The calorimeter is a box made of a given number of layers. A layer consists
/// of an absorber plate and of a detection gap. The layer is replicated.
///
/// Four parameters define the geometry of the calorimeter :
///
/// - the thickness of an absorber plate,
/// - the thickness of a gap,
/// - the number of layers,
/// - the transverse size of the calorimeter (the input face is a square).
///
/// In ConstructSDandField() sensitive detectors of CalorimeterSD type
/// are created and associated with the Absorber and Gap volumes.
/// In addition a transverse uniform magnetic field is defined
/// via G4GlobalMagFieldMessenger class.

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction();
    ~DetectorConstruction() override = default;

    G4VPhysicalVolume* Construct() override;
    void ConstructSDandField() override;

    void SetNewValueDouble(G4String, G4double);
    void SetNewValueInt(G4String, G4int);

    void UpdateGeometry();

private:
    // methods
    //
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();

    ZDCMaterials* fMaterials;
    G4LogicalVolume* fCoatingLogical_EM;
    G4LogicalVolume* fWLSLogical_EM;
    G4LogicalVolume* fPMTLogical_EM;
    G4LogicalVolume* fCrysLogical_EM;

    G4LogicalVolume* fWSci_WLogical;
    G4LogicalVolume* fWSci_SciLogical;
    G4LogicalVolume* fCoatingLogical_0;
    G4LogicalVolume* fReflectorLogical_0;
    G4LogicalVolume* fHoleLogical_0;
    G4LogicalVolume* fWLSLogical_0;
    G4LogicalVolume* fClad1Logical_0;
    G4LogicalVolume* fClad2Logical_0;
    G4LogicalVolume* fMirrorLogical_0;
    G4LogicalVolume* fSipmLogical_0;
    G4LogicalVolume* fRodHoleLogical_0;
    G4LogicalVolume* fRodLogical_0;

    G4LogicalVolume* fCoatingLogical_1;
    G4LogicalVolume* fFrontPlateLogical_1;
    G4LogicalVolume* fBackPlateLogical_1;
    G4LogicalVolume* fScinLogical_1;
    G4LogicalVolume* fLeadLogical_1;
    G4LogicalVolume* fReflectorLogical_1;
    G4LogicalVolume* fHoleLogical_1;
    G4LogicalVolume* fWLSLogical_1;
    G4LogicalVolume* fClad1Logical_1;
    G4LogicalVolume* fClad2Logical_1;
    G4LogicalVolume* fMirrorLogical_1;
    G4LogicalVolume* fSipmLogical_1;
    G4LogicalVolume* fRodHoleLogical_1;
    G4LogicalVolume* fRodLogical_1;

    G4LogicalVolume* fCoatingLogical_2;
    G4LogicalVolume* fFrontPlateLogical_2;
    G4LogicalVolume* fBackPlateLogical_2;
    G4LogicalVolume* fScinLogical_2;
    G4LogicalVolume* fLeadLogical_2;
    G4LogicalVolume* fReflectorLogical_2;
    G4LogicalVolume* fHoleLogical_2;
    G4LogicalVolume* fWLSLogical_2;
    G4LogicalVolume* fClad1Logical_2;
    G4LogicalVolume* fClad2Logical_2;
    G4LogicalVolume* fMirrorLogical_2;
    G4LogicalVolume* fSipmLogical_2;
    G4LogicalVolume* fRodHoleLogical_2;
    G4LogicalVolume* fRodLogical_2;
    
    // data members
    //
    static G4ThreadLocal G4GlobalMagFieldMessenger* fMagFieldMessenger;
    // magnetic field messenger

    G4bool fCheckOverlaps = true;  // option to activate checking of volumes overlaps
    G4int fNofLayers = -1;  // number of layers

    DetectorMessenger* fMessenger;

    // detector parameter start with v
    //Emc
    G4int    vNEMc = kNEMc;
    G4double vEMSize = kEMSize;
    G4double vEMArraySizeXY;
    G4double vEMLength = kEMLength;

    //WSci
    G4int    vNWSci = kNWSci;
    G4double vWSciSize = kWSciSize;
    G4double vWSciArraySize;
    G4int    vNWSciLayer = kNWSciLayer;
    G4double vWSciWThick = kWSciWThick;
    G4double vWSciSciThick = kWSciSciThick;
    G4double vWSciModuleLength;
    G4double vWSciFiberLength;
    G4double vWSciHoleLength;
    G4double vWSciRodLength;

    // Shashlik
    G4int vNShash = kNShash;
    G4int vNShashLayer = kNShashLayer;
    G4double vShashScinThick = kShashScinThick;
    G4double vShashLeadThick = kShashLeadThick;
    G4double vShashSize = kShashSize;
    G4double vShashArraySizeXY;
    G4double vShashModuleLength;
    G4double vShashFiberLength;
    G4double vShashHoleLength;
    G4double vShashRodLength;

    // Fiber
    G4int vNFiberHole_WSci = kNFiberHole;
    G4int vNFiberHole_Shash1 = kNFiberHole;
    G4int vNFiberHole_Shash2 = kNFiberHole;
    
    


};

}  // namespace ZDC

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
