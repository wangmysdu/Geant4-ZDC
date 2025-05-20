#ifndef EventMessenger_h
#define EventMessenger_h 1

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

class EventAction;

class EventMessenger : public G4UImessenger
{
  public:
    EventMessenger(EventAction*);
    ~EventMessenger() override;

    void SetNewValue(G4UIcommand*, G4String) override;

  private:
    EventAction* fEventAction = nullptr;

    G4UIdirectory* fDetDirectory = nullptr;

    G4UIcmdWithoutParameter *fInitGeo = nullptr;

    G4UIcmdWithALongInt* fNEmc = nullptr;
    G4UIcmdWithALongInt* fNWSci = nullptr;
    G4UIcmdWithALongInt* fNShash = nullptr;

    G4UIcmdWithAString* fTargMatCmd = nullptr;
    G4UIcmdWithAString* fChamMatCmd = nullptr;

    G4UIcmdWithADoubleAndUnit* fStepMaxCmd = nullptr;
    G4UIcmdWithADouble* fDoubleInput = nullptr;
};

}  // namespace ZDC
#endif
