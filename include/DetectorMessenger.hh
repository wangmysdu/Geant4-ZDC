#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithALongInt;
class G4UIcmdWithoutParameter; 
class G4UIcommand;

namespace ZDC
{

class DetectorConstruction;

class DetectorMessenger : public G4UImessenger
{
  public:
    DetectorMessenger(DetectorConstruction*);
    ~DetectorMessenger() override;

    void SetNewValue(G4UIcommand*, G4String) override;

  private:
    DetectorConstruction* fDetectorConstruction = nullptr;

    G4UIdirectory* fDetDirectory = nullptr;

    G4UIcmdWithoutParameter *fInitGeo = nullptr;

    //Emc
    G4UIcmdWithALongInt* vNEmc = nullptr;
    G4UIcmdWithADoubleAndUnit* vEMSize = nullptr;
    G4UIcmdWithADoubleAndUnit* vEMLength = nullptr;

    // WSci
    G4UIcmdWithALongInt* vNWSci = nullptr;
    G4UIcmdWithALongInt* vNWSciLayer = nullptr;
    G4UIcmdWithADoubleAndUnit* vWSciSize = nullptr;
    G4UIcmdWithADoubleAndUnit* vWSciWThick = nullptr;
    G4UIcmdWithADoubleAndUnit* vWSciSciThick = nullptr;

    // Shashlik
    G4UIcmdWithALongInt* vNShash = nullptr;
    G4UIcmdWithALongInt* vNShashLayer = nullptr;
    G4UIcmdWithADoubleAndUnit* vShashScinThick = nullptr;
    G4UIcmdWithADoubleAndUnit* vShashLeadThick = nullptr;
    G4UIcmdWithADoubleAndUnit* vShashSize = nullptr;

    G4UIcmdWithALongInt* vNFiberHole = nullptr;
};

}  // namespace ZDC
#endif
