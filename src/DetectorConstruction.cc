///
/* Structure: front plate + (reflector + scintillator + reflector + lead) + back plate
 *
 * Detector ID
 *
 * (Front) 2nd detector, 1st detector (End with Sipm)
 * ID:     2000000        1000000
 * 
 * Module:   10000*(i+1) 
 *
 * Layer:  
 *     Sintillator: 1000+i*10
 *     Lead:        2000+i*10
 *     relector 1:  3000+20*i
 *     relector 2:  3000+10+20*i
 *
 *
 *     TiO2 coating: 1
 *     Front-end aluminum plate: 2
 *     Back-end aluminum plate:  3
 *
 * fiber structure: hole -> clad2 -> clad1 ->core  
 *     hole:  10+i (16 in total)
 *     clad2: 4
 *     clad1: 5
 *     core:  6
 *     mirror: 30+i
 *
 *     SimPM: 50+i
 *
 *     Rod hole: 70+i
 *     Rod: 7
 *
 *  Front   0 1 2 3 4 5 6 7 8 9 10 11 12 13 14  Sipm
 *          ----------------------------------> z
 * 
 *
 *                Front View
 *                --- --- ---
 *   ^           | 6 | 7 | 8 |
 * y |            --- --- ---
 *   |           | 3 | 4 | 5 |
 *    ----> x     --- --- ---
 *               | 0 | 1 | 2 |
 *                --- --- ---
 *
*/ 

#include "DetectorConstruction.hh"
#include "CalorimeterSD.hh"
#include "SiPMSD.hh"
#include "DetectorMessenger.hh"

#include "ZDCMaterials.hh"
#include "G4VisAttributes.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"

#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "G4UserLimits.hh"

#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4GenericMessenger.hh"

#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"

#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4PhysicalConstants.hh"

namespace ZDC
{

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction()
{
    fMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4GlobalMagFieldMessenger* DetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
//  DefineMaterials();

  // Define volumes
//  return DefineVolumes();

  // Define materials 
    fMaterials = ZDCMaterials::GetInstance(); // initilize the material
    DefineMaterials();

    auto air = G4Material::GetMaterial("G4_AIR");
    auto scintillator = G4Material::GetMaterial("Polystyrene"); // or other
    auto lead = G4Material::GetMaterial("G4_Cu");
    auto TiO2 = G4Material::GetMaterial("G4_TITANIUM_DIOXIDE");
    auto silicon = G4Material::GetMaterial("G4_Si");
    auto aluminum = G4Material::GetMaterial("G4_Al");
    auto steel = G4Material::GetMaterial("G4_STAINLESS-STEEL");
    auto WLS = G4Material::GetMaterial("PMMA");
    auto clad1 = G4Material::GetMaterial("Pethylene");
    auto clad2 = G4Material::GetMaterial("FPethylene");
    auto tyvek = G4Material::GetMaterial("Tyvek");
    auto tungsten = G4Material::GetMaterial("G4_Cu");
    auto PbWO4 = G4Material::GetMaterial("G4_PbWO4");

    //para
    //Emc
    vEMArraySizeXY = vEMSize + 2 * kCoating;
    //WSci
    vWSciArraySize = vWSciSize + 2 * kWSciModuleGap;
    vWSciModuleLength = vNWSciLayer*(vWSciWThick + vWSciSciThick + 2.*kWSciLayerGap);
    vWSciFiberLength = vWSciModuleLength + kFiberFrontLength + kFiberBackLength;
    vWSciHoleLength = vWSciFiberLength + 1*cm;
    vWSciRodLength = vWSciModuleLength + 1*cm;
    // Shashlik
    vShashArraySizeXY = vShashSize + 2 * kCoating;
    vShashModuleLength = vNShashLayer * (vShashScinThick + vShashLeadThick + 2* kReflectorThick);
    vShashFiberLength = vShashModuleLength + kFiberFrontLength + kFiberBackLength;
    vShashHoleLength = vShashFiberLength + 1*cm;
    vShashRodLength = vShashModuleLength + 1*cm;

    // Option to switch on/off checking of volumes overlaps
    G4bool checkOverlaps = false;
    // geometries --------------------------------------------------------------
    // experimental hall (world volume)
    auto worldSolid = new G4Box("worldBox", 100. * cm, 100. * cm, 250 * cm);
    auto worldLogical = new G4LogicalVolume(worldSolid, air, "worldLogical");
    auto worldPhysical = new G4PVPlacement(0, G4ThreeVector(), worldLogical, "worldPhysical", 0, false, 0, checkOverlaps); 
    // false: (pMany), Currently NOT used. For future use to identify if the volume is meant to be considered an overlapping structure, or not.

    G4double SectorBoxGap = 1*mm;
    G4double CurrentZPos = -100*cm;
    G4double EmcLVBoxLength = (vEMLength + 4*cm);
// EM Module
    G4LogicalVolume *EM_array_LV = nullptr;
    if (UsePbWO4EMCal){
        G4Box *EM_fArray_Box = new G4Box("EM_Array_box", vEMArraySizeXY / 2, vEMArraySizeXY / 2, EmcLVBoxLength / 2.0); // its size
        EM_array_LV = new G4LogicalVolume(EM_fArray_Box, air, "EM_Array_LV");
        G4VPhysicalVolume *EM_fArray_Phy;
        for (G4int i = 0; i < vNEMc * vNEMc; i++)
        {
            G4double x_layer = -vEMArraySizeXY / 2 * (vNEMc - 1) + G4int(i % vNEMc) * vEMArraySizeXY;
            G4double y_layer = -vEMArraySizeXY / 2 * (vNEMc - 1) + G4int(i / vNEMc) * vEMArraySizeXY;
		    G4double z_layer = CurrentZPos;
            EM_fArray_Phy = new G4PVPlacement(0, G4ThreeVector(x_layer, y_layer,  z_layer), EM_array_LV, "EMLayer", worldLogical, false, (i+1)*10000+4000000);
        }
        G4cout << "Placed physical volume: " << EM_fArray_Phy->GetName() << G4endl;

        // filled with crystal
        auto fCrysSolid_EM = new G4Box("CrysBox_EM", vEMSize / 2., vEMSize / 2., vEMLength / 2.);
        fCrysLogical_EM = new G4LogicalVolume(fCrysSolid_EM, PbWO4, "CrysLogical_EM");
        G4VPhysicalVolume *fCrysPhysical_EM = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fCrysLogical_EM, "CrysPhysical_EM", EM_array_LV, false, 0*10+1000, checkOverlaps);
	
        // the TiO2 coating (If need the air gap)
        auto fCoatingSolid_original_EM = new G4Box("CoatingBox_original_EM", vEMArraySizeXY / 2., vEMArraySizeXY / 2., vEMLength/ 2.0 + kCoating);
        auto fCoatingSolid_sub_EM = new G4Box("CoatingBox_sub_EM", vEMSize / 2., vEMSize / 2.,  vEMLength/ 2.);
        auto fCoatingSolid_EM = new G4SubtractionSolid("CoatingBox_EM", fCoatingSolid_original_EM, fCoatingSolid_sub_EM);
        fCoatingLogical_EM = new G4LogicalVolume(fCoatingSolid_EM, tyvek, "CoatingLogical_EM");
        G4VPhysicalVolume *fCoatingPhysical_EM = new G4PVPlacement(0, G4ThreeVector(0, 0, 0*cm), fCoatingLogical_EM, "CoatingPhysical_EM", EM_array_LV, false, 1, checkOverlaps);
        G4cout << "Placed physical volume: " << fCoatingPhysical_EM->GetName() << G4endl;

        // (PMT or Sipm)
        auto fPMTSolid_EM = new G4Tubs("PMTTubs_EM", 0., kEMPMTSize / 2., kEMPMTThick / 2., 0, 360);
        fPMTLogical_EM = new G4LogicalVolume(fPMTSolid_EM, silicon, "PMTLogical_EM");
        G4VPhysicalVolume *fPMTPhysical_EM = new G4PVPlacement(0, G4ThreeVector(0, 0, vEMLength/2.0 + kEMPMTThick/2.), fPMTLogical_EM, "PMTPhysical_EM", EM_array_LV, false, 50, checkOverlaps);
    }
    
//WSci Module
    //G4double vWSciModuleLength = vNWSciLayer*(vWSciWThick + vWSciSciThick + 2.*kWSciLayerGap);
    G4double WSciBoxLength = (vWSciModuleLength/2+4*cm)*2.;
    CurrentZPos += EmcLVBoxLength/2. + SectorBoxGap + WSciBoxLength/2.;
    G4Box *fArray_Box_WSci = new G4Box("Array_box_WSci", vWSciArraySize / 2, vWSciArraySize / 2, WSciBoxLength/2.); // its size
    G4LogicalVolume *array_LV_WSci = new G4LogicalVolume(fArray_Box_WSci, air, "Array_LV_WSci");
    G4VPhysicalVolume *fArray_Phy_WSci;
    for (G4int i = 0; i < vNWSci * vNWSci; i++){
        G4double x_layer = -vWSciArraySize / 2 * (vNWSci - 1) + G4int(i % vNWSci) * vWSciArraySize;
        G4double y_layer = -vWSciArraySize / 2 * (vNWSci - 1) + G4int(i / vNWSci) * vWSciArraySize;
        G4double z_layer = CurrentZPos;
        fArray_Phy_WSci = new G4PVPlacement(0, G4ThreeVector(x_layer, y_layer,  z_layer), array_LV_WSci, "LayerWSci", worldLogical, false, (i+1)*10000+3000000);

    }
    G4cout << "Placed physical volume: " << fArray_Phy_WSci->GetName() << G4endl;

    // the TiO2 coating
    auto fCoatingSolid_original_0 = new G4Box("CoatingBox_original_0", vWSciArraySize / 2., vWSciArraySize / 2., vWSciModuleLength / 2.0 + kWSciModuleGap);
    auto fCoatingSolid_sub_0 = new G4Box("CoatingBox_sub_0", vWSciSize / 2., vWSciSize / 2., vWSciModuleLength / 2.);
    auto fCoatingSolid_0 = new G4SubtractionSolid("CoatingBox_0", fCoatingSolid_original_0, fCoatingSolid_sub_0);
    fCoatingLogical_0 = new G4LogicalVolume(fCoatingSolid_0, TiO2, "CoatingLogical_0");
    G4VPhysicalVolume *fCoatingPhysical_0 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fCoatingLogical_0, "CoatingPhysical_0", array_LV_WSci, false, 1, checkOverlaps);
    G4cout << "Placed physical volume: " << fCoatingPhysical_0->GetName() << G4endl;
        
    auto fWSolid = new G4Box("WBox", vWSciSize / 2., vWSciSize / 2., vWSciWThick / 2.);
    fWSci_WLogical = new G4LogicalVolume(fWSolid, tungsten, "WLogical");
    G4VPhysicalVolume *fWPhysical[vNWSciLayer];
    for (G4int i = 0; i < vNWSciLayer; i++){
        G4double z_lead = (vWSciSciThick + vWSciWThick + kWSciLayerGap * 2.0) * i + vWSciWThick / 2.0 - vWSciModuleLength/2.;
        fWPhysical[i] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_lead), fWSci_WLogical, "WPhysical", array_LV_WSci, false, i*10+2000, checkOverlaps);
    }
        
    auto fWScinSolid = new G4Box("WScinBox", vWSciSize / 2., vWSciSize / 2., vWSciSciThick / 2.);
    fWSci_SciLogical = new G4LogicalVolume(fWScinSolid, scintillator, "WScinLogical"); //replace silicon to scin
    G4VPhysicalVolume *fWScinPhysical;
    for (G4int i = 0; i < vNWSciLayer; i++){
            G4double z_scin = (vWSciSciThick + vWSciWThick + kWSciLayerGap * 2.0) * i + kWSciLayerGap + vWSciWThick + vWSciSciThick / 2.0 - vWSciModuleLength/2.;
            fWScinPhysical = new G4PVPlacement(0, G4ThreeVector(0, 0, z_scin), fWSci_SciLogical, "WScinPhysical", array_LV_WSci, false, i*10+1000, checkOverlaps);
    }

    // reflector
    auto fReflectorSolid_0 = new G4Box("ReflectorBox_0", vWSciSize / 2., vWSciSize / 2., kWSciLayerGap / 2.);
    fReflectorLogical_0 = new G4LogicalVolume(fReflectorSolid_0, tyvek, "ReflectorLogical_0");
    G4VPhysicalVolume *fReflectorPhysical_0[vNWSciLayer * 2];
    for (G4int i = 0; i < vNWSciLayer; i++){
        G4double z_reflector = (vWSciSciThick + vWSciWThick + kWSciLayerGap * 2.0) * i +  vWSciWThick + kWSciLayerGap /2. - vWSciModuleLength/2.;
        G4double z_reflector_2 = z_reflector + kWSciLayerGap + vWSciSciThick;
        fReflectorPhysical_0[i] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_reflector), fReflectorLogical_0, "ReflectorPhysical_0", array_LV_WSci, false, 10*i+3000, checkOverlaps);
        fReflectorPhysical_0[i + vNWSciLayer] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_reflector_2), fReflectorLogical_0, "ReflectorPhysical_0", array_LV_WSci, false, 10*i+3000, checkOverlaps);
    }

    // fibers
    // hole -> clad2 -> clad2 ->core
    // fiber hole
    auto fHoleSolid_0 = new G4Tubs("HoleTubs_0", 0., kHoleDiameter / 2., vWSciHoleLength / 2., 0, 360);
    fHoleLogical_0 = new G4LogicalVolume(fHoleSolid_0, air, "HoleLogical_0");
    G4VPhysicalVolume *fHolePhysical_0;
    G4int holesPerRow = std::sqrt(vNFiberHole_WSci);
    G4double step = vWSciSize / holesPerRow;
    G4double startOffset = step / 2.0;
    for (G4int i = 0; i < vNFiberHole_WSci; i++){
        G4double z_hole = (kFiberBackLength - kFiberFrontLength) / 2.;
        G4int col = i % holesPerRow;
        G4int row = i / holesPerRow;
        G4double x_hole = -(vWSciSize/2.0) + startOffset + col * step;
        G4double y_hole = -(vWSciSize/2.0) + startOffset + row * step;
        // G4double x_hole = -(vWSciSize / 2.0) + vWSciSize / 8.0 + G4int(i % 4) * vWSciSize / 4.0;
        // G4double y_hole = -(vWSciSize / 2.0) + vWSciSize / 8.0 + G4int(i / 4) * vWSciSize / 4.0;
        fHolePhysical_0 = new G4PVPlacement(0, G4ThreeVector(x_hole, y_hole, z_hole), fHoleLogical_0, "HolePhysical_0", array_LV_WSci, false, i+10, false);
    }

    // Clad2
    auto fClad2Solid_0 = new G4Tubs("Clad2Tubs_0", 0, kFiberClad2Diameter / 2., vWSciFiberLength / 2., 0, 360);
    fClad2Logical_0 = new G4LogicalVolume(fClad2Solid_0, clad2, "Clad2Logical_0");
    G4VPhysicalVolume *fClad2Physical_0 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fClad2Logical_0, "Clad2Physical_0", fHoleLogical_0, false, 4, checkOverlaps);

    //Clad1
    auto fClad1Solid_0 = new G4Tubs("Clad1Tubs_0", 0, kFiberClad1Diameter / 2., vWSciFiberLength / 2., 0, 360);
    fClad1Logical_0 = new G4LogicalVolume(fClad1Solid_0, clad1, "Clad1Logical_0");
    G4VPhysicalVolume *fClad1Physical_0 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fClad1Logical_0, "Clad1Physical_0", fClad2Logical_0, false, 5, checkOverlaps);

    // fiber core
    auto fWLSSolid_0 = new G4Tubs("WLSTubs_0", 0., kFiberWLSDiameter / 2., vWSciFiberLength / 2., 0, 360);
    fWLSLogical_0 = new G4LogicalVolume(fWLSSolid_0, WLS, "WLSLogical_0");
    G4VPhysicalVolume *fWLSPhysical_0 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fWLSLogical_0, "WLSPhysical_0", fClad1Logical_0, false, 6, checkOverlaps);

    // Mirror
    auto fMirrorSolid_0 = new G4Tubs("MirrorTubs_0", 0., kMirrorDiameter / 2., kMirrorThick / 2., 0, 360);
    fMirrorLogical_0 = new G4LogicalVolume(fMirrorSolid_0, aluminum, "MirrorLogical_0");
    G4double z_Mirror = - vWSciFiberLength / 2. - kMirrorThick / 2.0;
    G4VPhysicalVolume *fMirrorPhysical_0 = new G4PVPlacement(0, G4ThreeVector(0, 0, z_Mirror), fMirrorLogical_0, "MirrorPhysical_0", fHoleLogical_0, false, 30, checkOverlaps);
    // for (G4int i = 0; i < vNFiberHole; i++)
    // {
    //         //G4double z_Mirror = - kFiberFrontLength - kMirrorThick / 2.0 - vWSciModuleLength/2.;
    //         G4double z_Mirror = - vWSciFiberLength / 2. - kMirrorThick / 2.0;
    //         G4double x_Mirror = -(vWSciSize / 2.0) + vWSciSize / 8.0 + G4int(i % 4) * vWSciSize / 4.0;
    //         G4double y_Mirror = -(vWSciSizee / 2.0) + vWSciSize / 8.0 + G4int(i / 4) * vWSciSize / 4.0;
    //         //fMirrorPhysical_0[i] = new G4PVPlacement(0, G4ThreeVector(x_Mirror, y_Mirror, z_Mirror), fMirrorLogical_0, "MirrorPhysical_0", array_LV_WSci, false, i+30, checkOverlaps);
    //         fMirrorPhysical_0[i] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_Mirror), fMirrorLogical_0, "MirrorPhysical_0", fHoleLogical_0, false, i+30, checkOverlaps);
    // }

    // Sipm
    auto fSipmSolid_0 = new G4Box("SipmBox_0", kHoleDiameter / 2., kHoleDiameter / 2., kMirrorThick / 2.);
    fSipmLogical_0 = new G4LogicalVolume(fSipmSolid_0, silicon, "SipmLogical_0");
    G4double z_Sipm = vWSciFiberLength / 2. + kSipmThick / 2.0;
    G4VPhysicalVolume *fSipmPhysical_0 = new G4PVPlacement(0, G4ThreeVector(0, 0, z_Sipm), fSipmLogical_0, "SipmPhysical_0", fHoleLogical_0, false, 50, checkOverlaps);
    // for (G4int i = 0; i < vNFiberHole; i++)
    // {
    //         //G4double z_Sipm = vWSciModuleLength + kFiberBackLength  + kMirrorThick / 2. - vWSciModuleLength/2.;
    //         G4double z_Sipm = vWSciFiberLength / 2. + kSipmThick / 2.0;
    //         G4double x_Sipm = -(vWSciSize / 2.0) + vWSciSize / 8.0 + G4int(i % 4) * vWSciSize / 4.0;
    //         G4double y_Sipm = -(vWSciSize / 2.0) + vWSciSize / 8.0 + G4int(i / 4) * vWSciSize / 4.0;
    //         fSipmPhysical_0[i] = new G4PVPlacement(0, G4ThreeVector(x_Sipm, y_Sipm, z_Sipm), fSipmLogical_0, "SipmPhysical_0", array_LV_WSci, false, i+50, checkOverlaps);
    //         fSipmPhysical_0[i] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_Sipm), fSipmLogical_0, "SipmPhysical_0", fHoleLogical_0, false, i+50, checkOverlaps);

    // }

    // Rod
    // rod hole
    auto fRodHoleSolid_0 = new G4Tubs("RodHoleTubs_0", 0., kRodHoleDiameter / 2., vWSciRodLength / 2., 0, 360);
    fRodHoleLogical_0 = new G4LogicalVolume(fRodHoleSolid_0, air, "RodHoleLogical_0");
    G4VPhysicalVolume *fRodHolePhysical_0;
    for (G4int i = 0; i < kNofRods; i++){
        G4double z_RodHole = vShashModuleLength/2 - vShashModuleLength/2.;
        G4double x_RodHole = -(vWSciSize / 2.0) + vWSciSize / 4.0 + G4int(i % 2) * vWSciSize / 2.0;
        G4double y_RodHole = -(vWSciSize / 2.0) + vWSciSize / 4.0 + G4int(i / 2) * vWSciSize / 2.0;
        fRodHolePhysical_0 = new G4PVPlacement(0, G4ThreeVector(x_RodHole, y_RodHole, z_RodHole), fRodHoleLogical_0, "RodHolePhysical_0", array_LV_WSci, false, i+70, false);
    }

    // rod
    auto fRodSolid_0 = new G4Tubs("RodTubs_0", 0., kRodDiameter / 2., vWSciRodLength / 2., 0, 360);
    fRodLogical_0 = new G4LogicalVolume(fRodSolid_0, steel, "RodLogical_0");
    G4VPhysicalVolume *fRodPhysical_0 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fRodLogical_0, "RodPhysical_0", fRodHoleLogical_0, false, 7, checkOverlaps);
    G4cout << "Placed physical volume: " << fRodSolid_0->GetName() << G4endl;
    G4cout << "Placed physical volume: " << fRodPhysical_0->GetName() << G4endl;

    
    // ZDC Module
    // module array; notice: array length is twice of module, aim to put the front layer in center
    G4double PbSciBoxLength = (vShashModuleLength/2.+5*cm)*2.;
    CurrentZPos += WSciBoxLength/2. + SectorBoxGap + PbSciBoxLength/2.;
    G4Box *fArray_Box_1 = new G4Box("Array_box_1", vShashArraySizeXY / 2, vShashArraySizeXY / 2, PbSciBoxLength/2.); // its size
    G4LogicalVolume *array_LV_1 = new G4LogicalVolume(fArray_Box_1, air, "Array_LV_1");
    G4VPhysicalVolume *fArray_Phy_1;
    for (G4int i = 0; i < vNShash* vNShash; i++){
        G4double x_layer = -vShashArraySizeXY / 2 * (vNShash- 1) + G4int(i % vNShash) * vShashArraySizeXY;
        G4double y_layer = -vShashArraySizeXY / 2 * (vNShash- 1) + G4int(i / vNShash) * vShashArraySizeXY;
	    G4double z_layer = CurrentZPos;
        fArray_Phy_1 = new G4PVPlacement(0, G4ThreeVector(x_layer, y_layer,  z_layer), array_LV_1, "Layer1", worldLogical, false, (i+1)*10000+1000000);

    }
    G4cout << "Placed physical volume: " << fArray_Phy_1->GetName() << G4endl;

    // the TiO2 coating
    auto fCoatingSolid_original_1 = new G4Box("CoatingBox_original_1", vShashArraySizeXY / 2., vShashArraySizeXY / 2., vShashModuleLength / 2.0);
    auto fCoatingSolid_sub_1 = new G4Box("CoatingBox_sub_1", vShashSize / 2., vShashSize / 2., vShashModuleLength / 2.);
    auto fCoatingSolid_1 = new G4SubtractionSolid("CoatingBox_1", fCoatingSolid_original_1, fCoatingSolid_sub_1);
    fCoatingLogical_1 = new G4LogicalVolume(fCoatingSolid_1, TiO2, "CoatingLogical_1");
    G4VPhysicalVolume *fCoatingPhysical_1 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fCoatingLogical_1, "CoatingPhysical_1", array_LV_1, false, 1, checkOverlaps);
    G4cout << "Placed physical volume: " << fCoatingPhysical_1->GetName() << G4endl;

    // lyaers /
    // front plate + (reflector + scintillator + reflector + lead) + back plate
    // Front-end aluminum plate
    auto fFrontPlateSolid_1 = new G4Box("FrontPlateBox_1", vShashSize / 2., vShashSize / 2., kFrontPlateThick / 2.);
    fFrontPlateLogical_1 = new G4LogicalVolume(fFrontPlateSolid_1, aluminum, "FrontPlateLogical_1");
    G4VPhysicalVolume *fFrontPlatePhysical_1 = 
    new G4PVPlacement(0, G4ThreeVector(0, 0, -vShashModuleLength / 2. - kFrontPlateThick/2.), fFrontPlateLogical_1, "FrontPlatePhysical_1", array_LV_1, false, 2, checkOverlaps);
    G4cout << "Placed physical volume: " << fFrontPlatePhysical_1->GetName() << G4endl;

    // scintillator layer
    auto fScinSolid_1 = new G4Box("ScinBox_1", vShashSize / 2., vShashSize / 2., vShashScinThick / 2.);
    fScinLogical_1 = new G4LogicalVolume(fScinSolid_1, scintillator, "ScinLogical_1");
    G4VPhysicalVolume *fScinPhysical_1;
    for (G4int i = 0; i < vNShashLayer; i++){
        G4double z_scin = (vShashScinThick + vShashLeadThick + kReflectorThick * 2.0) * i + kReflectorThick + vShashLeadThick + kFrontPlateThick + vShashScinThick / 2.0 - vShashModuleLength/2.;
        fScinPhysical_1 = new G4PVPlacement(0, G4ThreeVector(0, 0, z_scin), fScinLogical_1, "ScinPhysical_1", array_LV_1, false, i*10+1000, checkOverlaps);
    }

    // lead
    auto fLeadSolid_1 = new G4Box("LeadBox", vShashSize / 2., vShashSize / 2., vShashLeadThick / 2.);
    fLeadLogical_1 = new G4LogicalVolume(fLeadSolid_1, lead, "LeadLogical_1");
    G4VPhysicalVolume *fLeadPhysical_1[vNShashLayer];
    for (G4int i = 0; i < vNShashLayer; i++){
        G4double z_lead = (vShashScinThick + vShashLeadThick + kReflectorThick * 2.0) * i + kFrontPlateThick + vShashLeadThick / 2.0 - vShashModuleLength/2.;
        fLeadPhysical_1[i] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_lead), fLeadLogical_1, "LeadPhysical_1", array_LV_1, false, i*10+2000, checkOverlaps);
    }

    // reflector
    auto fReflectorSolid_1 = new G4Box("ReflectorBox_1", vShashSize / 2., vShashSize / 2., kReflectorThick / 2.);
    fReflectorLogical_1 = new G4LogicalVolume(fReflectorSolid_1, tyvek, "ReflectorLogical_1");
    G4VPhysicalVolume *fReflectorPhysical_1[vNShashLayer * 2];
    for (G4int i = 0; i < vNShashLayer; i++){
        G4double z_reflector = (vShashScinThick + vShashLeadThick + kReflectorThick * 2.0) * i + kFrontPlateThick + vShashLeadThick + kReflectorThick / 2.0 - vShashModuleLength/2.;
        G4double z_reflector_2 = z_reflector + vShashScinThick + kReflectorThick;
        fReflectorPhysical_1[i] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_reflector), fReflectorLogical_1, "ReflectorPhysical_1", array_LV_1, false, 20*i+3000, checkOverlaps);
        fReflectorPhysical_1[i + vNShashLayer] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_reflector_2), fReflectorLogical_1, "ReflectorPhysical_1", array_LV_1, false, 20*i+10+3000, checkOverlaps);
    }

    // Back-end aluminum plate
    auto fBackPlateSolid_1 = new G4Box("FrontPlateBox_1", vShashSize / 2., vShashSize / 2., kBackPlateThick / 2.);
    fBackPlateLogical_1 = new G4LogicalVolume(fFrontPlateSolid_1, aluminum, "BackPlateLogical_1");
    G4VPhysicalVolume *fBackPlatePhysical_1 = 
    new G4PVPlacement(0, G4ThreeVector(0, 0, vShashModuleLength/2 + kBackPlateThick / 2.), fBackPlateLogical_1, "BackPlatePhysical_1", array_LV_1, false, 3, checkOverlaps);

    G4cout << "Placed physical volume: " << fBackPlateSolid_1->GetName() << G4endl;
    G4cout << "Placed physical volume: " << fBackPlatePhysical_1->GetName() << G4endl;

    // fibers
    // hole -> clad2 -> clad2 ->core
    // fiber hole
    auto fHoleSolid_1 = new G4Tubs("HoleTubs_1", 0., kHoleDiameter / 2., vShashHoleLength / 2., 0, 360);
    fHoleLogical_1 = new G4LogicalVolume(fHoleSolid_1, air, "HoleLogical_1");
    G4VPhysicalVolume *fHolePhysical_1;
    G4int holesPerRow_1 = std::sqrt(vNFiberHole_Shash1);
    G4double step_1 = vShashSize / holesPerRow_1;
    G4double startOffset_1 = step_1 / 2.0;
    for (G4int i = 0; i < vNFiberHole_Shash1; i++) {
        G4int col = i % holesPerRow_1;
        G4int row = i / holesPerRow_1;
        G4double x_hole = -(vShashSize/2.0) + startOffset_1 + col * step_1;
        G4double y_hole = -(vShashSize/2.0) + startOffset_1 + row * step_1;
        G4double z_hole = (kFiberBackLength - kFiberFrontLength) / 2.;
        // G4double x_hole = -(vShashSize / 2.0) + vShashSize / 8.0 + G4int(i % 4) * vShashSize / 4.0;
        // G4double y_hole = -(vShashSize / 2.0) + vShashSize / 8.0 + G4int(i / 4) * vShashSize / 4.0;
        fHolePhysical_1 = new G4PVPlacement(0, G4ThreeVector(x_hole, y_hole, z_hole), fHoleLogical_1, "HolePhysical_1", array_LV_1, false, i+10, false);
    }

    // Clad2
    auto fClad2Solid_1 = new G4Tubs("Clad2Tubs_1", 0, kFiberClad2Diameter / 2., vShashFiberLength / 2., 0, 360);
    fClad2Logical_1 = new G4LogicalVolume(fClad2Solid_1, clad2, "Clad2Logical_1");
    G4VPhysicalVolume *fClad2Physical_1 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fClad2Logical_1, "Clad2Physical_1", fHoleLogical_1, false, 4, checkOverlaps);

    //Clad1
    auto fClad1Solid_1 = new G4Tubs("Clad1Tubs_1", 0, kFiberClad1Diameter / 2., vShashFiberLength / 2., 0, 360);
    fClad1Logical_1 = new G4LogicalVolume(fClad1Solid_1, clad1, "Clad1Logical_1");
    G4VPhysicalVolume *fClad1Physical_1 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fClad1Logical_1, "Clad1Physical_1", fClad2Logical_1, false, 5, checkOverlaps);

    // fiber core
    auto fWLSSolid_1 = new G4Tubs("WLSTubs_1", 0., kFiberWLSDiameter / 2., vShashFiberLength / 2., 0, 360);
    fWLSLogical_1 = new G4LogicalVolume(fWLSSolid_1, WLS, "WLSLogical_1");
    G4VPhysicalVolume *fWLSPhysical_1 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fWLSLogical_1, "WLSPhysical_1", fClad1Logical_1, false, 6, checkOverlaps);

    // Mirror
    auto fMirrorSolid_1 = new G4Tubs("MirrorTubs_1", 0., kMirrorDiameter / 2., kMirrorThick / 2., 0, 360);
    fMirrorLogical_1 = new G4LogicalVolume(fMirrorSolid_1, aluminum, "MirrorLogical_1");
    z_Mirror = - vShashFiberLength / 2. - kMirrorThick / 2.0;
    G4VPhysicalVolume *fMirrorPhysical_1 = new G4PVPlacement(0, G4ThreeVector(0, 0, z_Mirror), fMirrorLogical_1, "MirrorPhysical_1", fHoleLogical_1, false, 30, checkOverlaps);
    // for (G4int i = 0; i < vNFiberHole; i++)
    // {
    //         //G4double z_Mirror = - kFiberFrontLength - kMirrorThick / 2.0 + vShashModuleLength/2.;
    //         G4double z_Mirror = - vShashFiberLength / 2. - kMirrorThick / 2.0;
    //         G4double x_Mirror = -(vShashSize / 2.0) + vShashSize / 8.0 + G4int(i % 4) * vShashSize / 4.0;
    //         G4double y_Mirror = -(vShashSize / 2.0) + vShashSize / 8.0 + G4int(i / 4) * vShashSize / 4.0;
    //         //fMirrorPhysical_1[i] = new G4PVPlacement(0, G4ThreeVector(x_Mirror, y_Mirror, z_Mirror), fMirrorLogical_1, "MirrorPhysical_1", array_LV_1, false, i+30, checkOverlaps);
    //         fMirrorPhysical_1[i] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_Mirror), fMirrorLogical_1, "MirrorPhysical_1", fHoleLogical_1, false, i+30, checkOverlaps);
    // }

    // Sipm
    auto fSipmSolid_1 = new G4Box("SipmBox_1", kHoleDiameter / 2., kHoleDiameter / 2., kMirrorThick / 2.);
    fSipmLogical_1 = new G4LogicalVolume(fSipmSolid_1, silicon, "SipmLogical_1");
    z_Sipm = vShashFiberLength / 2. + kSipmThick / 2.0;
    G4VPhysicalVolume *fSipmPhysical_1 = new G4PVPlacement(0, G4ThreeVector(0, 0, z_Sipm), fSipmLogical_1, "SipmPhysical_1", fHoleLogical_1, false, 50, checkOverlaps);
    // for (G4int i = 0; i < vNFiberHole; i++)
    // {
    //         G4double z_Sipm = vShashModuleLength + kFiberBackLength  + kMirrorThick / 2. - vShashModuleLength/2.;
    //         G4double x_Sipm = -(vShashSize / 2.0) + vShashSize / 8.0 + G4int(i % 4) * vShashSize / 4.0;
    //         G4double y_Sipm = -(vShashSize / 2.0) + vShashSize / 8.0 + G4int(i / 4) * vShashSize / 4.0;
    //         fSipmPhysical_1[i] = new G4PVPlacement(0, G4ThreeVector(x_Sipm, y_Sipm, z_Sipm), fSipmLogical_1, "SipmPhysical_1", array_LV_1, false, i+50, checkOverlaps);
    // }

    // Rod
    // rod hole
    auto fRodHoleSolid_1 = new G4Tubs("RodHoleTubs_1", 0., kRodHoleDiameter / 2., vShashRodLength / 2., 0, 360);
    fRodHoleLogical_1 = new G4LogicalVolume(fRodHoleSolid_1, air, "RodHoleLogical_1");
    G4VPhysicalVolume *fRodHolePhysical_1;
    for (G4int i = 0; i < kNofRods; i++){
            G4double z_RodHole = vShashModuleLength/2 - vShashModuleLength/2.;
            G4double x_RodHole = -(vShashSize / 2.0) + vShashSize / 4.0 + G4int(i % 2) * vShashSize / 2.0;
            G4double y_RodHole = -(vShashSize / 2.0) + vShashSize / 4.0 + G4int(i / 2) * vShashSize / 2.0;
            fRodHolePhysical_1 = new G4PVPlacement(0, G4ThreeVector(x_RodHole, y_RodHole, z_RodHole), fRodHoleLogical_1, "RodHolePhysical_1", array_LV_1, false, i+70, false);
    }

    // rod
    auto fRodSolid_1 = new G4Tubs("RodTubs_1", 0., kRodDiameter / 2., vShashRodLength / 2., 0, 360);
    fRodLogical_1 = new G4LogicalVolume(fRodSolid_1, steel, "RodLogical_1");
    G4VPhysicalVolume *fRodPhysical_1 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fRodLogical_1, "RodPhysical_1", fRodHoleLogical_1, false, 7, checkOverlaps);
    G4cout << "Placed physical volume: " << fRodSolid_1->GetName() << G4endl;
    G4cout << "Placed physical volume: " << fRodPhysical_1->GetName() << G4endl;

////////////////// Make a second Copy:
    // module array; notice: array length is twice of module, aim to put the front layer in center
    CurrentZPos += PbSciBoxLength/2. + SectorBoxGap + PbSciBoxLength/2.;
    G4Box *fArray_Box_2 = new G4Box("Array_box_2", vShashArraySizeXY / 2, vShashArraySizeXY / 2, PbSciBoxLength/2.); // its size
    G4LogicalVolume *array_LV_2 = new G4LogicalVolume(fArray_Box_2, air, "Array_LV_2");
    G4VPhysicalVolume *fArray_Phy_2;
    for (G4int i = 0; i < vNShash* vNShash; i++){
        G4double x_layer = -vShashArraySizeXY / 2 * (vNShash- 1) + G4int(i % vNShash) * vShashArraySizeXY;
        G4double y_layer = -vShashArraySizeXY / 2 * (vNShash- 1) + G4int(i / vNShash) * vShashArraySizeXY;
	    G4double z_layer = CurrentZPos;
        fArray_Phy_2 = new G4PVPlacement(0, G4ThreeVector(x_layer, y_layer, z_layer), array_LV_2, "Layer2", worldLogical, false, (i+1)*10000+2000000);
    }
    G4cout << "Placed physical volume: " << fArray_Phy_2->GetName() << G4endl;

    // the TiO2 coating
    auto fCoatingSolid_original_2 = new G4Box("CoatingBox_original_2", vShashArraySizeXY / 2., vShashArraySizeXY / 2., vShashModuleLength / 2.0);
    auto fCoatingSolid_sub_2 = new G4Box("CoatingBox_sub_2", vShashSize / 2., vShashSize / 2., vShashModuleLength / 2.);
    auto fCoatingSolid_2 = new G4SubtractionSolid("CoatingBox_2", fCoatingSolid_original_2, fCoatingSolid_sub_2);
    fCoatingLogical_2 = new G4LogicalVolume(fCoatingSolid_2, TiO2, "CoatingLogical_2");
    G4VPhysicalVolume *fCoatingPhysical_2 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fCoatingLogical_2, "CoatingPhysical_2", array_LV_2, false, 1, checkOverlaps);
    G4cout << "Placed physical volume: " << fCoatingPhysical_2->GetName() << G4endl;

    // layers
    // front plate + (reflector + scintillator + reflector + lead) + back plate
    // Front-end aluminum plate
    auto fFrontPlateSolid_2 = new G4Box("FrontPlateBox_2", vShashSize / 2., vShashSize / 2., kFrontPlateThick / 2.);
    fFrontPlateLogical_2 = new G4LogicalVolume(fFrontPlateSolid_2, aluminum, "FrontPlateLogical_2");
    G4VPhysicalVolume *fFrontPlatePhysical_2 = 
    new G4PVPlacement(0, G4ThreeVector(0, 0, kFrontPlateThick / 2. - vShashModuleLength / 2.), fFrontPlateLogical_2, "FrontPlatePhysical_2", array_LV_2, false, 2, checkOverlaps);
    G4cout << "Placed physical volume: " << fFrontPlatePhysical_2->GetName() << G4endl;

    // scintillator layer
    auto fScinSolid_2 = new G4Box("ScinBox_2", vShashSize / 2., vShashSize / 2., vShashScinThick / 2.);
    fScinLogical_2 = new G4LogicalVolume(fScinSolid_2, scintillator, "ScinLogical_2");
    G4VPhysicalVolume *fScinPhysical_2;
    for (G4int i = 0; i < vNShashLayer; i++){
        G4double z_scin = (vShashScinThick + vShashLeadThick + kReflectorThick * 2.0) * i + kReflectorThick + vShashLeadThick + kFrontPlateThick + vShashScinThick / 2.0 -vShashModuleLength / 2.;
        fScinPhysical_2 = new G4PVPlacement(0, G4ThreeVector(0, 0, z_scin), fScinLogical_2, "ScinPhysical_2", array_LV_2, false, i*10+1000, checkOverlaps);
    }
    G4cout << "Placed physical volume: " << fScinPhysical_2->GetName() << G4endl;

    // lead
    auto fLeadSolid_2 = new G4Box("LeadBox", vShashSize / 2., vShashSize / 2., vShashLeadThick / 2.);
    fLeadLogical_2 = new G4LogicalVolume(fLeadSolid_2, lead, "LeadLogical_2");
    G4VPhysicalVolume *fLeadPhysical_2[vNShashLayer];
    for (G4int i = 0; i < vNShashLayer; i++){
        G4double z_lead = (vShashScinThick + vShashLeadThick + kReflectorThick * 2.0) * i + kFrontPlateThick + vShashLeadThick / 2.0 -vShashModuleLength / 2.;
        fLeadPhysical_2[i] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_lead), fLeadLogical_2, "LeadPhysical_2", array_LV_2, false, i*10+2000, checkOverlaps);
    }

    // reflector
    auto fReflectorSolid_2 = new G4Box("ReflectorBox_2", vShashSize / 2., vShashSize / 2., kReflectorThick / 2.);
    fReflectorLogical_2 = new G4LogicalVolume(fReflectorSolid_2, tyvek, "ReflectorLogical_2");
    G4VPhysicalVolume *fReflectorPhysical_2[vNShashLayer * 2];
    for (G4int i = 0; i < vNShashLayer; i++){
        G4double z_reflector = (vShashScinThick + vShashLeadThick + kReflectorThick * 2.0) * i + kFrontPlateThick + vShashLeadThick + kReflectorThick / 2.0 -vShashModuleLength / 2.;
        G4double z_reflector_2 = z_reflector + vShashScinThick + kReflectorThick;
        fReflectorPhysical_2[i] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_reflector), fReflectorLogical_2, "ReflectorPhysical_2", array_LV_2, false, 20*i+3000, checkOverlaps);
        fReflectorPhysical_2[i + vNShashLayer] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_reflector_2), fReflectorLogical_2, "ReflectorPhysical_2", array_LV_2, false, 20*i+10+3000, checkOverlaps);
    }

    // Back-end aluminum plate
    auto fBackPlateSolid_2 = new G4Box("FrontPlateBox_2", vShashSize / 2., vShashSize / 2., kBackPlateThick / 2.);
    fBackPlateLogical_2 = new G4LogicalVolume(fFrontPlateSolid_2, aluminum, "BackPlateLogical_2");
    G4VPhysicalVolume *fBackPlatePhysical_2 = 
    new G4PVPlacement(0, G4ThreeVector(0, 0, vShashModuleLength/2 - kBackPlateThick / 2.), fBackPlateLogical_2, "BackPlatePhysical_2", array_LV_2, false, 3, checkOverlaps);
    G4cout << "Placed physical volume: " << fBackPlateSolid_2->GetName() << G4endl;
    G4cout << "Placed physical volume: " << fBackPlateLogical_2->GetName() << G4endl;
    G4cout << "Placed physical volume: " << fBackPlatePhysical_2->GetName() << G4endl;

    // fibers
    // hole -> clad2 -> clad2 ->core
    // fiber hole
    auto fHoleSolid_2 = new G4Tubs("HoleTubs_2", 0., kHoleDiameter / 2., vShashHoleLength / 2., 0, 360);
    fHoleLogical_2 = new G4LogicalVolume(fHoleSolid_2, air, "HoleLogical_2");
    G4VPhysicalVolume *fHolePhysical_2;
    G4int holesPerRow_2 = std::sqrt(vNFiberHole_Shash2);
    G4double step_2 = vShashSize / holesPerRow_2;
    G4double startOffset_2 = step_2 / 2.0;
    for (G4int i = 0; i < vNFiberHole_Shash2; i++) {
        G4int col = i % holesPerRow_2;
        G4int row = i / holesPerRow_2;
        G4double x_hole = -(vShashSize/2.0) + startOffset_2 + col * step_2;
        G4double y_hole = -(vShashSize/2.0) + startOffset_2 + row * step_2;
        G4double z_hole = (kFiberBackLength - kFiberFrontLength) / 2.;
        // G4double x_hole = -(vShashSize / 2.0) + vShashSize / 8.0 + G4int(i % 4) * vShashSize / 4.0;
        // G4double y_hole = -(vShashSize / 2.0) + vShashSize / 8.0 + G4int(i / 4) * vShashSize / 4.0;
        fHolePhysical_2 = new G4PVPlacement(0, G4ThreeVector(x_hole, y_hole, z_hole), fHoleLogical_2, "HolePhysical_2", array_LV_2, false, i+10, false);
    }
    G4cout << "Placed physical volume: " << fHolePhysical_2->GetName() << G4endl;

    // Clad2
    auto fClad2Solid_2 = new G4Tubs("Clad2Tubs_2", 0, kFiberClad2Diameter / 2., vShashFiberLength / 2., 0, 360);
    fClad2Logical_2 = new G4LogicalVolume(fClad2Solid_2, clad2, "Clad2Logical_2");
    G4VPhysicalVolume *fClad2Physical_2 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fClad2Logical_2, "Clad2Physical_2", fHoleLogical_2, false, 4, checkOverlaps);
    G4cout << "Placed physical volume: " << fClad2Physical_2->GetName() << G4endl;

    //Clad1
    auto fClad1Solid_2 = new G4Tubs("Clad1Tubs_2", 0, kFiberClad1Diameter / 2., vShashFiberLength / 2., 0, 360);
    fClad1Logical_2 = new G4LogicalVolume(fClad1Solid_2, clad1, "Clad1Logical_2");
    G4VPhysicalVolume *fClad1Physical_2 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fClad1Logical_2, "Clad1Physical_2", fClad2Logical_2, false, 5, checkOverlaps);
    G4cout << "Placed physical volume: " << fClad1Physical_2->GetName() << G4endl;

    // fiber core
    auto fWLSSolid_2 = new G4Tubs("WLSTubs_2", 0., kFiberWLSDiameter / 2., vShashFiberLength / 2., 0, 360);
    fWLSLogical_2 = new G4LogicalVolume(fWLSSolid_2, WLS, "WLSLogical_2");
    G4VPhysicalVolume *fWLSPhysical_2 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fWLSLogical_2, "WLSPhysical_2", fClad1Logical_2, false, 6, checkOverlaps);
    G4cout << "Placed physical volume: " << fWLSPhysical_2->GetName() << G4endl;

    // Mirror
    auto fMirrorSolid_2 = new G4Tubs("MirrorTubs_2", 0., kMirrorDiameter / 2., kMirrorThick / 2., 0, 360);
    fMirrorLogical_2 = new G4LogicalVolume(fMirrorSolid_2, aluminum, "MirrorLogical_2");
    z_Mirror = - vShashFiberLength / 2. - kMirrorThick / 2.0;
    G4VPhysicalVolume *fMirrorPhysical_2 = new G4PVPlacement(0, G4ThreeVector(0, 0, z_Mirror), fMirrorLogical_2, "MirrorPhysical_2", fHoleLogical_2, false, 30, checkOverlaps);
        // for (G4int i = 0; i < vNFiberHole; i++)
        // {
        //         //G4double z_Mirror = - kFiberFrontLength - kMirrorThick / 2.0 + vShashModuleLength / 2.;
        //         G4double z_Mirror = - vShashFiberLength / 2. - kMirrorThick / 2.0;
        //         G4double x_Mirror = -(vShashSize / 2.0) + vShashSize / 8.0 + G4int(i % 4) * vShashSize / 4.0;
        //         G4double y_Mirror = -(vShashSize / 2.0) + vShashSize / 8.0 + G4int(i / 4) * vShashSize / 4.0;
        //         //fMirrorPhysical_2[i] = new G4PVPlacement(0, G4ThreeVector(x_Mirror, y_Mirror, z_Mirror), fMirrorLogical_2, "MirrorPhysical_2", array_LV_2, false, i+30, checkOverlaps);
        //         fMirrorPhysical_2[i] = new G4PVPlacement(0, G4ThreeVector(0, 0, z_Mirror), fMirrorLogical_2, "MirrorPhysical_2", fHoleLogical_2, false, i+30, checkOverlaps);
        // }

    // Sipm
    auto fSipmSolid_2 = new G4Box("SipmBox_2", kHoleDiameter / 2., kHoleDiameter / 2., kMirrorThick / 2.);
    fSipmLogical_2 = new G4LogicalVolume(fSipmSolid_2, silicon, "SipmLogical_2");
    z_Sipm = vShashFiberLength / 2. + kSipmThick / 2.0;
    G4VPhysicalVolume *fSipmPhysical_2 = new G4PVPlacement(0, G4ThreeVector(0, 0, z_Sipm), fSipmLogical_2, "SipmPhysical_2", fHoleLogical_2, false, 50, checkOverlaps);
    // for (G4int i = 0; i < vNFiberHole; i++)
    // {
    //         G4double z_Sipm = vShashModuleLength + kFiberBackLength  + kMirrorThick / 2. -vShashModuleLength / 2.;
    //         G4double x_Sipm = -(vShashSize / 2.0) + vShashSize / 8.0 + G4int(i % 4) * vShashSize / 4.0;
    //         G4double y_Sipm = -(vShashSize / 2.0) + vShashSize / 8.0 + G4int(i / 4) * vShashSize / 4.0;
    //         fSipmPhysical_2[i] = new G4PVPlacement(0, G4ThreeVector(x_Sipm, y_Sipm, z_Sipm), fSipmLogical_2, "SipmPhysical_2", array_LV_2, false, i+50, checkOverlaps);
    // }

    //Rod
    // rod hole
    auto fRodHoleSolid_2 = new G4Tubs("RodHoleTubs_2", 0., kRodHoleDiameter / 2., vShashRodLength / 2., 0, 360);
    fRodHoleLogical_2 = new G4LogicalVolume(fRodHoleSolid_2, air, "RodHoleLogical_2");
    G4VPhysicalVolume *fRodHolePhysical_2;
    for (G4int i = 0; i < kNofRods; i++){
        G4double z_RodHole = 0;
        G4double x_RodHole = -(vShashSize / 2.0) + vShashSize / 4.0 + G4int(i % 2) * vShashSize / 2.0;
        G4double y_RodHole = -(vShashSize / 2.0) + vShashSize / 4.0 + G4int(i / 2) * vShashSize / 2.0;
        fRodHolePhysical_2 = new G4PVPlacement(0, G4ThreeVector(x_RodHole, y_RodHole, z_RodHole), fRodHoleLogical_2, "RodHolePhysical_2", array_LV_2, false, i+70, false);
    }
    G4cout << "Placed physical volume: " << fRodHolePhysical_2->GetName() << G4endl;

    // rod
    auto fRodSolid_2 = new G4Tubs("RodTubs_2", 0., kRodDiameter / 2., vShashRodLength / 2., 0, 360);
    fRodLogical_2 = new G4LogicalVolume(fRodSolid_2, steel, "RodLogical_2");
    G4VPhysicalVolume *fRodPhysical_2 = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), fRodLogical_2, "RodPhysical_2", fRodHoleLogical_2, false, 7, checkOverlaps);
    G4cout << "Placed physical volume: " << fRodPhysical_2->GetName() << G4endl;

////////////////////////////
////////////////////////////
        
    // Drawing
    // visualization attributes ------------------------------------------------
    G4VisAttributes *transgrey = new G4VisAttributes(G4Colour(0.64, 0.7, 0.7, 0.2));
    transgrey->SetForceSolid(true);
    G4VisAttributes *transblue = new G4VisAttributes(G4Colour(0.01, 0.98, 0.9, 0.5));
    transblue->SetForceSolid(true);
    transblue->SetVisibility(true);
    transblue->SetLineWidth(2);
    G4VisAttributes *transyellow = new G4VisAttributes(G4Colour(0.8, 0.9, 0.1, 0.3));
    transyellow->SetForceSolid(true);
    transyellow->SetForceSolid(true);
    transyellow->SetLineWidth(2);
    G4VisAttributes *transgreen = new G4VisAttributes(G4Colour(0.2, 0.98, 0.1, 0.4));
    transgreen->SetForceSolid(true);
    G4VisAttributes *transred = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.4));
    transred->SetForceSolid(true);
    G4VisAttributes *transwhite = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
    transwhite->SetForceSolid(true);
    G4VisAttributes *solidblue = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0));
    solidblue->SetForceSolid(true);
    G4VisAttributes *solidgreen = new G4VisAttributes(G4Color::Green());
    solidgreen->SetForceSolid(true);
    G4VisAttributes *linegrey = new G4VisAttributes(G4Colour(0.64, 0.7, 0.7, 0.2));
    linegrey->SetLineWidth(1);

    auto visAttributes = new G4VisAttributes(G4Colour(0, 0, 0, 0));
    //worldLogical->SetVisAttributes(G4VisAttributes::GetInvisible());
    worldLogical->SetVisAttributes(linegrey);
    
    if (UsePbWO4EMCal) {
        fCoatingLogical_EM->SetVisAttributes(G4VisAttributes::GetInvisible());
        EM_array_LV->SetVisAttributes(linegrey);
        fCrysLogical_EM->SetVisAttributes(transblue);
    }

    fWSci_WLogical->SetVisAttributes(transred);
    fWSci_SciLogical->SetVisAttributes(transblue);
    fReflectorLogical_0->SetVisAttributes(G4VisAttributes::GetInvisible());
    fCoatingLogical_0->SetVisAttributes(G4VisAttributes::GetInvisible());
    fClad2Logical_0->SetVisAttributes(transgreen);
    fMirrorLogical_0->SetVisAttributes(transyellow);
    fHoleLogical_0->SetVisAttributes(G4VisAttributes::GetInvisible());


    array_LV_1->SetVisAttributes(linegrey);
    fCoatingLogical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    fFrontPlateLogical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    fBackPlateLogical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    fScinLogical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    fLeadLogical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    fScinLogical_1->SetVisAttributes(transblue);
    fLeadLogical_1->SetVisAttributes(transred); 	
    fReflectorLogical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    //fReflectorLogical_1->SetVisAttributes(transyellow);
    fHoleLogical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    //fWLSLogical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    fWLSLogical_1->SetVisAttributes(transgreen);
    fClad1Logical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    fClad2Logical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    //fMirrorLogical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    //fSipmLogical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    fSipmLogical_1->SetVisAttributes(solidgreen);
    fRodHoleLogical_1->SetVisAttributes(G4VisAttributes::GetInvisible());
    fRodLogical_1->SetVisAttributes(G4VisAttributes::GetInvisible());

    array_LV_2->SetVisAttributes(linegrey);
    fCoatingLogical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    fFrontPlateLogical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    fBackPlateLogical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    //        fScinLogical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    fScinLogical_2->SetVisAttributes(transblue);
    //        fLeadLogical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    fLeadLogical_2->SetVisAttributes(transred);        
    fReflectorLogical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    fHoleLogical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    //        fWLSLogical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    fWLSLogical_2->SetVisAttributes(transgreen);
    fClad1Logical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    fClad2Logical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    //fMirrorLogical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    //        fSipmLogical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    fSipmLogical_2->SetVisAttributes(solidgreen);
    fRodHoleLogical_2->SetVisAttributes(G4VisAttributes::GetInvisible());
    fRodLogical_2->SetVisAttributes(G4VisAttributes::GetInvisible());

//        fSipmLogical_EM->SetVisAttributes(G4VisAttributes::GetInvisible());
//        fCrysLogical_EM->SetVisAttributes(G4VisAttributes::GetInvisible());

    array_LV_WSci->SetVisAttributes(linegrey);

    // Scintillator - World
    G4OpticalSurface *opSurface_ScinAir = new G4OpticalSurface("Surface_ScinAir",     // Surface Name
            glisur,                           // SetModel, define Setfinish, LUT not installed ???????????
            polishedair,      // SetFinish (polished or ground) polished improve Nphotons significant
            dielectric_dielectric, // Set Type
            polished);                        // vlaue

    new G4LogicalBorderSurface("ScinSurface_0", fWScinPhysical, worldPhysical, opSurface_ScinAir);  //实际没有接触，以防万一
    new G4LogicalBorderSurface("WorldScinSurface_1", worldPhysical, fWScinPhysical, opSurface_ScinAir);
    new G4LogicalBorderSurface("ScinSurface_1", fScinPhysical_1, worldPhysical, opSurface_ScinAir);  
    new G4LogicalBorderSurface("WorldScinSurface_1", worldPhysical, fScinPhysical_1, opSurface_ScinAir);
    new G4LogicalBorderSurface("ScinSurface_2", fScinPhysical_2, worldPhysical, opSurface_ScinAir); 
    new G4LogicalBorderSurface("WorldScinSurface_2", worldPhysical, fScinPhysical_2, opSurface_ScinAir);

    // Scintillator - Hole
    new G4LogicalBorderSurface("HoleScinSurface_0", fHolePhysical_0, fWScinPhysical, opSurface_ScinAir);
    new G4LogicalBorderSurface("ScinHoleSurface_0", fWScinPhysical, fHolePhysical_0, opSurface_ScinAir);
    new G4LogicalBorderSurface("RodHoleScinSurface_0", fRodHolePhysical_0, fWScinPhysical, opSurface_ScinAir);
    new G4LogicalBorderSurface("ScinRodHoleSurface_0", fWScinPhysical, fRodHolePhysical_0, opSurface_ScinAir);
    new G4LogicalBorderSurface("HoleScinSurface_", fHolePhysical_1, fScinPhysical_1, opSurface_ScinAir);
    new G4LogicalBorderSurface("ScinHoleSurface_1", fScinPhysical_1, fHolePhysical_1, opSurface_ScinAir);
    new G4LogicalBorderSurface("RodHoleScinSurface_1", fRodHolePhysical_1, fScinPhysical_1, opSurface_ScinAir);
    new G4LogicalBorderSurface("ScinRodHoleSurface_1", fScinPhysical_1, fRodHolePhysical_1, opSurface_ScinAir);
    new G4LogicalBorderSurface("HoleScinSurface_2", fHolePhysical_2, fScinPhysical_2, opSurface_ScinAir);
    new G4LogicalBorderSurface("ScinHoleSurface_2", fScinPhysical_2, fHolePhysical_2, opSurface_ScinAir);
    new G4LogicalBorderSurface("RodHoleScinSurface_2", fRodHolePhysical_2, fScinPhysical_2, opSurface_ScinAir);
    new G4LogicalBorderSurface("ScinRodHoleSurface_2", fScinPhysical_2, fRodHolePhysical_2, opSurface_ScinAir);

        // WLS
    G4OpticalSurface *opSurface_fiber = new G4OpticalSurface("Surface_fiber",               // Surface Name
            glisur,                         // SetModel, define Setfinish
            polished,                       // SetFinish (polished or ground) polished improve Nphotons significan; polishedAir work bad
            dielectric_dielectric, // SetType
            polished);                              // vlaue

    // Clad2 - Air(hole)
    new G4LogicalBorderSurface("Clad2AirSurface_0", fClad2Physical_0, fHolePhysical_0, opSurface_fiber);
    new G4LogicalBorderSurface("AirClad2Surface_0", fHolePhysical_0, fClad2Physical_0, opSurface_fiber);
    new G4LogicalBorderSurface("Clad2AirSurface_1", fClad2Physical_1, fHolePhysical_1, opSurface_fiber);
    new G4LogicalBorderSurface("AirClad2Surface_1", fHolePhysical_1, fClad2Physical_1, opSurface_fiber);
    new G4LogicalBorderSurface("Clad2AirSurface_2", fClad2Physical_2, fHolePhysical_2, opSurface_fiber);
    new G4LogicalBorderSurface("AirClad2Surface_2", fHolePhysical_2, fClad2Physical_2, opSurface_fiber);

    // Clad2 - Clad1
    new G4LogicalBorderSurface("Clad2Clad1Surface_0", fClad2Physical_0, fClad1Physical_0, opSurface_fiber);
    new G4LogicalBorderSurface("Clad1Clad2Surface_0", fClad1Physical_0, fClad2Physical_0, opSurface_fiber);
    new G4LogicalBorderSurface("Clad2Clad1Surface_1", fClad2Physical_1, fClad1Physical_1, opSurface_fiber);
    new G4LogicalBorderSurface("Clad1Clad2Surface_1", fClad1Physical_1, fClad2Physical_1, opSurface_fiber);
    new G4LogicalBorderSurface("Clad2Clad1Surface_2", fClad2Physical_2, fClad1Physical_2, opSurface_fiber);
    new G4LogicalBorderSurface("Clad1Clad2Surface_2", fClad1Physical_2, fClad2Physical_2, opSurface_fiber);

    // WLS - Clad1
    new G4LogicalBorderSurface("WLSClad1Surface_0", fWLSPhysical_0, fClad1Physical_0, opSurface_fiber);
    new G4LogicalBorderSurface("Clad1WLSSurface_0", fClad1Physical_0, fWLSPhysical_0, opSurface_fiber);
    new G4LogicalBorderSurface("WLSClad1Surface_1", fWLSPhysical_1, fClad1Physical_1, opSurface_fiber);
    new G4LogicalBorderSurface("Clad1WLSSurface_1", fClad1Physical_1, fWLSPhysical_1, opSurface_fiber);
    new G4LogicalBorderSurface("WLSClad1Surface_2", fWLSPhysical_2, fClad1Physical_2, opSurface_fiber);
    new G4LogicalBorderSurface("Clad1WLSSurface_2", fClad1Physical_2, fWLSPhysical_2, opSurface_fiber);

    ////////////////////////////////////////
    // metal-dielectric: tyvek, TiO2, Mirror, Sipm

    //tyvek
    G4OpticalSurface *TyvekSurface = fMaterials->GetTyvekSurface();
    if (UsePbWO4EMCal) new G4LogicalSkinSurface("TyvekSurface_EM", fCoatingLogical_EM, TyvekSurface);
    new G4LogicalSkinSurface("TyvekSurface_1", fReflectorLogical_0, TyvekSurface);
    new G4LogicalSkinSurface("TyvekSurface_1", fReflectorLogical_1, TyvekSurface);
    new G4LogicalSkinSurface("TyvekSurface_2", fReflectorLogical_2, TyvekSurface);

    // TiO2: coating
    G4OpticalSurface *TiO2Surface = new G4OpticalSurface("TiO2Surface",
            glisur,
            ground,
            dielectric_metal,
            0.1);

    G4MaterialPropertiesTable *TiO2SurfaceProperty = new G4MaterialPropertiesTable();

    G4double p_TiO2[2] = {2.00 * eV, 3.47 * eV};
    const G4int nbins = 2;

    G4double refl_TiO2[2] = {0.9, 0.9};
    TiO2SurfaceProperty->AddProperty("REFLECTIVITY", p_TiO2, refl_TiO2, nbins);
    TiO2Surface->SetMaterialPropertiesTable(TiO2SurfaceProperty);

    new G4LogicalSkinSurface("CoatingSurface_0", fCoatingLogical_0, TiO2Surface);
    new G4LogicalSkinSurface("CoatingSurface_1", fCoatingLogical_1, TiO2Surface); //why add this, the result become non-physical??? seem OK now.
    new G4LogicalSkinSurface("CoatingSurface_2", fCoatingLogical_2, TiO2Surface);

    // Mirror
    G4OpticalSurface *MirrorSurface = new G4OpticalSurface("MirrorSurface",
            glisur,
            ground,
            dielectric_metal,
            1.);  //测试表明影响不是很大??

    G4MaterialPropertiesTable *MirrorSurfaceProperty = new G4MaterialPropertiesTable();

    G4double p_Mirror[2] = {2.00 * eV, 3.47 * eV};
    G4double refl_Mirror[2] = {0.9, 0.9};
    G4double effi_Mirror[2] = {1-refl_Mirror[0], 1-refl_Mirror[1]};
    MirrorSurfaceProperty->AddProperty("REFLECTIVITY", p_Mirror, refl_Mirror, nbins);
    MirrorSurfaceProperty->AddProperty("EFFICIENCY", p_Mirror, effi_Mirror, nbins);
    MirrorSurface->SetMaterialPropertiesTable(MirrorSurfaceProperty);

    new G4LogicalSkinSurface("MirrorSurface_0", fMirrorLogical_0, MirrorSurface);
    new G4LogicalSkinSurface("MirrorSurface_1", fMirrorLogical_1, MirrorSurface);
    new G4LogicalSkinSurface("MirrorSurface_2", fMirrorLogical_2, MirrorSurface);

    // Sipm 
    G4OpticalSurface *SipmSurface = fMaterials->GetSipmSurface();
    new G4LogicalSkinSurface("SipmSurface_0", fSipmLogical_0, SipmSurface);
    new G4LogicalSkinSurface("SipmSurface_1", fSipmLogical_1, SipmSurface);
    new G4LogicalSkinSurface("SipmSurface_2", fSipmLogical_2, SipmSurface);
    if (UsePbWO4EMCal) new G4LogicalSkinSurface("PMTSurface", fPMTLogical_EM, SipmSurface);


    // return the world physical volume ----------------------------------------
    return worldPhysical;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
    // Lead material defined using NIST Manager
    auto nistManager = G4NistManager::Instance();
    //  nistManager->FindOrBuildMaterial("G4_Pb");

    // Air
    nistManager->FindOrBuildMaterial("G4_AIR");
    // Scintillator
    nistManager->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    // Lead
    nistManager->FindOrBuildMaterial("G4_Pb");
    // SiO2
    nistManager->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    // Al
    nistManager->FindOrBuildMaterial("G4_Al");
    // Si
    nistManager->FindOrBuildMaterial("G4_Si");
    // steel
    nistManager->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    // PMMA
    nistManager->FindOrBuildMaterial("PMMA");
    // Pethylene
    nistManager->FindOrBuildMaterial("Pethylene");
    // FPethylene
    nistManager->FindOrBuildMaterial("FPethylene");
    // TiO2
    nistManager->FindOrBuildMaterial("G4_TITANIUM_DIOXIDE");
    // Tyvek
    nistManager->FindOrBuildMaterial("Tyvek");
    // Tungstate
    nistManager->FindOrBuildMaterial("G4_W");
    // Iron
    nistManager->FindOrBuildMaterial("G4_Fe");
    // PbWO4
    nistManager->FindOrBuildMaterial("G4_PbWO4");
    // Cu
    nistManager->FindOrBuildMaterial("G4_Cu");

    // Liquid argon material
    G4double a;  // mass of a mole;
    G4double z;  // z=mean number of protons;  
    G4double density;
    new G4Material("liquidArgon", z=18., a= 39.95*g/mole, density= 1.390*g/cm3);
    // The argon by NIST Manager is a gas with a different density

    // Vacuum
    new G4Material("Galactic", z=1., a=1.01*g/mole,density= universe_mean_density,
                   kStateGas, 2.73*kelvin, 3.e-18*pascal);

    // Print materials
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
    // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
    static G4ThreadLocal G4bool sd_initialized = false; //在此设置静态变量，确保每个线程SD只初始化一次，否则报错。该函数每个线程都会调用，为了确保每个线程都运行一次，设为G4ThreadLocal量；如果设为全局变量，则只有第一个线程运行一次，其它线程报错。重要！！！

    // 
    // Sensitive detectors
    //
    if(!sd_initialized){
        auto absoSD1 = new CalorimeterSD("AbsorberSD1", "AbsorberHitsCollection1");
        G4SDManager::GetSDMpointer()->AddNewDetector(absoSD1);

        if (UsePbWO4EMCal)
        SetSensitiveDetector("CrysLogical_EM",  absoSD1);

        SetSensitiveDetector("WLogical",        absoSD1);
        SetSensitiveDetector("WScinLogical",    absoSD1);

        SetSensitiveDetector("ScinLogical_1",   absoSD1);
        SetSensitiveDetector("LeadLogical_1",   absoSD1);

        SetSensitiveDetector("ScinLogical_2",   absoSD1);
        SetSensitiveDetector("LeadLogical_2",   absoSD1);


        auto SipmSD2 = new SipmSD("SipmSD2", "SipmHitsCollection2");
        G4SDManager::GetSDMpointer()->AddNewDetector(SipmSD2);
        if (UsePbWO4EMCal) SetSensitiveDetector("PMTLogical_EM",SipmSD2); //use string name
        SetSensitiveDetector("SipmLogical_0",   SipmSD2);
        SetSensitiveDetector("SipmLogical_1",   SipmSD2);
        SetSensitiveDetector("SipmLogical_2",   SipmSD2);
    
        // 
        // Magnetic field
        //
        // Create global magnetic field messenger.
        // Uniform magnetic field is then created automatically if
        // the field value is not zero.
        G4ThreeVector fieldValue;
        fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
        fMagFieldMessenger->SetVerboseLevel(1);

        // Register the field messenger for deleting
        G4AutoDelete::Register(fMagFieldMessenger);
        sd_initialized = true;
    }
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetNewValueInt(G4String key, G4int value){
    if(key == "NEmc"){
        vNEMc = value;
        G4cout << "Set NEmc value = " << value << G4endl;
    }
    else if(key == "NWSci"){
        vNWSci = value;
        G4cout << "Set NWSci value = " << value << G4endl;
    }
    else if(key == "NShash"){
        vNShash = value;
        G4cout << "Set NShash value = " << value << G4endl;
    }
    else if(key == "NWSciLayer"){
        vNWSciLayer = value;
        G4cout << "Set NWSciLayer value = " << value << G4endl;
    }
    else if(key == "NShashLayer"){
        vNShashLayer = value;
        G4cout << "Set NShashLayer value = " << value << G4endl;
    }
    else if(key == "NFiberHole_WSci"){
        vNFiberHole_WSci = value;
        G4cout << "Set NFiberHole(WSci) value = " << value << G4endl;
    }
    else if(key == "NFiberHole_Shash1"){
        vNFiberHole_Shash1 = value;
        G4cout << "Set NFiberHole value = " << value << G4endl;
    }
    else if(key == "NFiberHole_Shash2"){
        vNFiberHole_Shash2= value;
        G4cout << "Set NFiberHole value = " << value << G4endl;
    }
    G4RunManager::GetRunManager()->GeometryHasBeenModified(); // 是一个用于标记几何结构已被修改的方法。它是用来通知 Geant4 几何发生了变化，需要在下一个run中重新初始化几何结构。 告知 Geant4 需要更新几何
}

void DetectorConstruction::SetNewValueDouble(G4String key, G4double value){
    if(key == "EMSize"){
        vEMSize = value;
        G4cout << "Set EMSize value = " << value << G4endl;
    }
    else if(key == "EMLength"){
        vEMLength = value;
        G4cout << "Set EMLength value = " << value << G4endl;
    }
    else if(key == "WSciSize"){
        vWSciSize = value;
        G4cout << "Set WSciSize value = " << value << G4endl;
    }
    else if(key == "WSciWThick"){
        vWSciWThick = value;
        G4cout << "Set WSciWThick value = " << value << G4endl;
    }
    else if(key == "WSciSciThick"){
        vWSciSciThick = value;
        G4cout << "Set WSciWThick value = " << value << G4endl;
    }
    else if(key == "WSciSize"){
        vWSciSize = value;
        G4cout << "Set WSciSize value = " << value << G4endl;
    }
    else if(key == "ShashScinThick"){
        vShashScinThick = value;
        G4cout << "Set ShashScinThick value = " << value << G4endl;
    }
    else if(key == "ShashLeadThick"){
        vShashLeadThick = value;
        G4cout << "Set ShashLeadThick value = " << value << G4endl;
    }
    else if(key == "ShashSize"){
        vShashSize = value;
        G4cout << "Set vhashSize value = " << value << G4endl;
    }
    G4RunManager::GetRunManager()->GeometryHasBeenModified(); // 是一个用于标记几何结构已被修改的方法。它是用来通知 Geant4 几何发生了变化，需要在下一个run中重新初始化几何结构。 告知 Geant4 需要更新几何
}


void DetectorConstruction::UpdateGeometry() {
    G4RunManager::GetRunManager()->ReinitializeGeometry();
}

}  // namespace ZDC
