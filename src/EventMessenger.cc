#include "EventMessenger.hh"

#include "EventAction.hh"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithALongInt.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIdirectory.hh"


namespace ZDC
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventMessenger::EventMessenger(EventAction* Evt) : fEventAction(Evt)
{
  fDetDirectory = new G4UIdirectory("/det/");
  fDetDirectory->SetGuidance("Event Action Control"); 

  fNEmc = new G4UIcmdWithALongInt("/det/Emc/NModule", this);
  fNEmc->AvailableForStates(G4State_PreInit, G4State_Idle);  //could work when "before detector Init + run process idel"

  fNWSci = new G4UIcmdWithALongInt("/det/WSci/NModule", this);
  fNWSci->AvailableForStates(G4State_PreInit, G4State_Idle);

  fNShash = new G4UIcmdWithALongInt("/det/Shash/NModule", this);
  fNShash->AvailableForStates(G4State_PreInit, G4State_Idle); 

  // fDoubleInput = new G4UIcmdWithADouble("/det/setDValue", this);
  // fDoubleInput->SetGuidance("Set a Double value");
  // fDoubleInput->SetParameterName("DValue", false);
  // fDoubleInput->SetRange("DValue > 0");
  // fDoubleInput->AvailableForStates(G4State_Idle); // set the state could be run with command, this state means idle of simulation
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventMessenger::~EventMessenger()
{
  delete fTargMatCmd;
  delete fChamMatCmd;
  delete fStepMaxCmd;
  delete fDoubleInput;
  delete fDetDirectory;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fNEmc) {
    fEventAction->SetNewValueInt("NEmc", fNEmc->GetNewLongIntValue(newValue));
  }

  if (command == fNWSci) {
    fEventAction->SetNewValueInt("NWSci", fNWSci->GetNewLongIntValue(newValue));
  }

  if (command == fNShash) {
    fEventAction->SetNewValueInt("NShash", fNShash->GetNewLongIntValue(newValue));
  }
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
