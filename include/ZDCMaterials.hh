#ifndef ZDCMaterials_h
#define ZDCMaterials_h 1

#include "globals.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"

namespace ZDC
{

class ZDCMaterials
{
public:
    virtual ~ZDCMaterials();

    static ZDCMaterials *GetInstance();

    G4Material *GetMaterial(const G4String);

    G4OpticalSurface *GetTyvekSurface() { return TyvekSurface; }  // surface 需要用函数获得
    G4OpticalSurface *GetPMTSurface() { return PMTSurface; }
    G4OpticalSurface *GetSipmSurface() { return SipmSurface; }

private:
    ZDCMaterials();

    void CreateMaterials();

private:
    static ZDCMaterials *fInstance;

    G4NistManager *fNistMan;

    G4Material *fAir;
    G4Material *fTyvek;
    G4Material *fNylon;
    G4Material *fTiO2; 
    G4Material *fCsI; 
    G4Material *fPbWO4;

    G4Material *fPMMA;        //光纤芯
    G4Material *fPethylene;   //clad1
    G4Material *fFPethylene;  //clad2
    G4Material *fPolystyrene; //聚苯乙烯，（闪烁体）
    G4Material *fSilicone;
    G4Material *fCoating;
    G4Material *fPhotocathode;

    G4Material *fEpoxy;
    G4Material *fCarbonFiber;

    G4Material *fLeadGlass;

    G4OpticalSurface* TyvekSurface;
    G4OpticalSurface* SipmSurface;
    G4OpticalSurface* PMTSurface;
};
}
#endif /*ZDCMaterials_h*/
