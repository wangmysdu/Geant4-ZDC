#ifndef B5Constants_h
#define B5Constants_h 1

#include "globals.hh"
#include "G4SystemOfUnits.hh"

constexpr G4bool UsePbWO4EMCal = false;

constexpr G4int  NSector = 3;
constexpr G4int kNShash = 4;
constexpr G4int kNEMc = 4;
constexpr G4int kNWSci = 5;

constexpr G4double kFiberFrontLength = 0.5 *cm;
constexpr G4double kFiberBackLength = 2 *cm;
constexpr G4double kCoating = 0.0025 *cm;
constexpr G4double kReflectorThick = 0.007 *cm; //cm,70um

// Emc
constexpr G4double kEMSize = 2.2 *cm;
//constexpr G4double EMkArraySizeXY = EMkSize + 2 * EMkCoating; //cm
constexpr G4double kEMLength = 7 *cm;
constexpr G4double kEMPMTSize = 1.5 *cm;
constexpr G4double kEMPMTThick = 0.2 *cm;

// WSci
constexpr G4double kWSciModuleGap = kCoating;
constexpr G4double kWSciSize = 5.0*cm;
//constexpr G4double kWSciSize = WSciModuleSize + 2*WSciModuleGap;
constexpr G4int    kNWSciLayer = 42;
constexpr G4double kWSciWThick = 3.5*mm;
constexpr G4double kWSciSciThick = 1.0*mm;
constexpr G4double kWSciLayerGap = kReflectorThick; //same as reflector 
//constexpr G4double kWSciModuleLength = kNofWSciLayer*(WSciWThick + WSciSciThick + 2.*WSciLayerGap);
//constexpr G4double kWSciFiberLength = WSciModuleLength + kFiberFrontLength + kFiberBackLength;
//constexpr G4double kWSciHoleLength = kWSciFiberLength +1*cm;
//constexpr G4double kWSciRodLength = WSciModuleLength +1*cm;

//Shashlik
constexpr G4int kNShashLayer = 15;
constexpr G4double kShashScinThick = 0.6 *cm; //cm
constexpr G4double kShashLeadThick = 2.4 *cm; //cm
constexpr G4double kShashSize = 12.0 *cm;
//constexpr G4double kShashArraySizeXY = kSize + 2 * kCoating;
constexpr G4double kFrontPlateThick = 0.5 *cm; //cm
constexpr G4double kBackPlateThick = 0.5 *cm; //cm

// Fiber
constexpr G4int kNFiberHole = 16;
//constexpr G4double kFiberLength = kModuleLength + kFiberFrontLength + kFiberBackLength;
//constexpr G4double kHoleLength = kFiberLength + 1*cm;

constexpr G4double kFiberClad2Diameter = 0.12  *cm; //1.2mm diameter, choice: 1.5/1.2/1 mm
constexpr G4double kHoleDiameter = kFiberClad2Diameter + 0.03*cm;; //cm
constexpr G4double kFiberWLSDiameter = kFiberClad2Diameter -  0.01*cm;
constexpr G4double kFiberClad1Diameter = kFiberClad2Diameter -  0.005*cm;


constexpr G4double kMirrorThick = 0.015  *cm; //cm
constexpr G4double kMirrorDiameter = kHoleDiameter; 
constexpr G4double kSipmThick = 0.015  *cm; 

constexpr G4int kNofRods = 4;
//constexpr G4double kRodLength = kModuleLength + 1.*cm;
constexpr G4double kRodHoleDiameter = 0.18 *cm; //cm
constexpr G4double kRodDiameter = 0.15 *cm;

#endif
