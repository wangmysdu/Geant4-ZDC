#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithALongInt.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIdirectory.hh"


namespace ZDC
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* Det) : fDetectorConstruction(Det)
{
  fDetDirectory = new G4UIdirectory("/det/");
  fDetDirectory->SetGuidance("Detector construction control");

  fInitGeo = new G4UIcmdWithoutParameter("/det/InitGeo", this);
  fInitGeo->AvailableForStates(G4State_PreInit, G4State_Idle); 

  // Emc
  vNEmc = new G4UIcmdWithALongInt("/det/Emc/NModule", this);
  vNEmc->AvailableForStates(G4State_PreInit, G4State_Idle);  //could work when "before detector Init + run process idel"

  vEMSize = new G4UIcmdWithADoubleAndUnit("/det/Emc/Size", this);
  vEMSize->AvailableForStates(G4State_PreInit, G4State_Idle); 

  vEMLength = new G4UIcmdWithADoubleAndUnit("/det/Emc/Length", this);
  vEMLength->AvailableForStates(G4State_PreInit, G4State_Idle); 

  // WSci
  vNWSci = new G4UIcmdWithALongInt("/det/WSci/NModule", this);
  vNWSci->AvailableForStates(G4State_PreInit, G4State_Idle);

  vNWSciLayer = new G4UIcmdWithALongInt("/det/WSci/NLayer", this);
  vNWSciLayer->AvailableForStates(G4State_PreInit, G4State_Idle);

  vWSciSize = new G4UIcmdWithADoubleAndUnit("/det/WSci/Size", this);
  vWSciSize->AvailableForStates(G4State_PreInit, G4State_Idle); 

  vWSciWThick = new G4UIcmdWithADoubleAndUnit("/det/WSci/WThick", this);
  vWSciWThick->AvailableForStates(G4State_PreInit, G4State_Idle); 

  vWSciSciThick = new G4UIcmdWithADoubleAndUnit("/det/WSci/SciThick", this);
  vWSciSciThick->AvailableForStates(G4State_PreInit, G4State_Idle); 

  // Shashlik
  vNShash = new G4UIcmdWithALongInt("/det/Shash/NModule", this);
  vNShash->AvailableForStates(G4State_PreInit, G4State_Idle); 

  vNShashLayer = new G4UIcmdWithALongInt("/det/Shash/NLayer", this);
  vNShashLayer->AvailableForStates(G4State_PreInit, G4State_Idle); 

  vShashScinThick = new G4UIcmdWithADoubleAndUnit("/det/Shash/ScinThick", this);
  vShashScinThick->AvailableForStates(G4State_PreInit, G4State_Idle); 

  vShashLeadThick = new G4UIcmdWithADoubleAndUnit("/det/Shash/LeadThick", this);
  vShashLeadThick->AvailableForStates(G4State_PreInit, G4State_Idle); 

  vShashSize = new G4UIcmdWithADoubleAndUnit("/det/Shash/Size", this);
  vShashSize->AvailableForStates(G4State_PreInit, G4State_Idle); 

  //Fiber
  vNFiberHole_WSci = new G4UIcmdWithALongInt("/det/NFiberHole_WSci", this);
  vNFiberHole_WSci->AvailableForStates(G4State_PreInit, G4State_Idle); 

  vNFiberHole_Shash1 = new G4UIcmdWithALongInt("/det/NFiberHole_Shash1", this);
  vNFiberHole_Shash1->AvailableForStates(G4State_PreInit, G4State_Idle); 

  vNFiberHole_Shash2 = new G4UIcmdWithALongInt("/det/NFiberHole_Shash2", this);
  vNFiberHole_Shash2->AvailableForStates(G4State_PreInit, G4State_Idle); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
  if (command == fInitGeo) {
    fDetectorConstruction->UpdateGeometry();
  }

  //Emc
  else if (command == vNEmc) {
    fDetectorConstruction->SetNewValueInt("NEmc", vNEmc->GetNewLongIntValue(newValue));
  }
  else if (command == vEMSize) {
    fDetectorConstruction->SetNewValueDouble("EMSize", vEMSize->GetNewDoubleValue(newValue));
  }
  else if (command == vEMLength) {
    fDetectorConstruction->SetNewValueDouble("EMLength", vEMLength->GetNewDoubleValue(newValue));
  }

  //WSci
  else if (command == vNWSci) {
    fDetectorConstruction->SetNewValueInt("NWSci", vNWSci->GetNewLongIntValue(newValue));
  }
  else if (command == vNWSciLayer) {
    fDetectorConstruction->SetNewValueInt("NWSciLayer", vNWSciLayer->GetNewLongIntValue(newValue));
  }
  else if (command == vWSciSize) {
    fDetectorConstruction->SetNewValueDouble("WSciSize", vWSciSize->GetNewDoubleValue(newValue));
  }
  else if (command == vWSciWThick) {
    fDetectorConstruction->SetNewValueDouble("WSciWThick", vWSciWThick->GetNewDoubleValue(newValue));
  }
  else if (command == vWSciSciThick) {
    fDetectorConstruction->SetNewValueDouble("WSciSciThick", vWSciSciThick->GetNewDoubleValue(newValue));
  }

  //Shash
  else if (command == vNShash) {
    fDetectorConstruction->SetNewValueInt("NShash", vNShash->GetNewLongIntValue(newValue));
  }
  else if (command == vNShashLayer) {
    fDetectorConstruction->SetNewValueInt("NShashLayer", vNShashLayer->GetNewLongIntValue(newValue));
  }
  else if (command == vShashScinThick) {
    fDetectorConstruction->SetNewValueDouble("ShashScinThick", vShashScinThick->GetNewDoubleValue(newValue));
  }
  else if (command == vShashLeadThick) {
    fDetectorConstruction->SetNewValueDouble("ShashLeadThick", vShashLeadThick->GetNewDoubleValue(newValue));
  }
  else if (command == vShashSize) {
    fDetectorConstruction->SetNewValueDouble("ShashSize", vShashSize->GetNewDoubleValue(newValue));
  }

  //Fiber
  else if (command == vNFiberHole_WSci) {
    fDetectorConstruction->SetNewValueInt("NFiberHole_WSci", vNFiberHole_WSci->GetNewLongIntValue(newValue));
  }
  else if (command == vNFiberHole_Shash1) {
    fDetectorConstruction->SetNewValueInt("NFiberHole_Shash1", vNFiberHole_Shash1->GetNewLongIntValue(newValue));
  }
  else if (command == vNFiberHole_Shash2) {
    fDetectorConstruction->SetNewValueInt("NFiberHole_Shash2", vNFiberHole_Shash2->GetNewLongIntValue(newValue));
  }


  // if (command == fDoubleInput) {
  //   fDetectorConstruction->SetDetectorValue(fDoubleInput->GetNewDoubleValue(newValue));
  // }
  // if (command == fDoubleInput) {
  //   fDetectorConstruction->UpdateGeometry(); // 重新构建几何
  // }
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
