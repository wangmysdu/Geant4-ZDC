#ifndef ZDCPrimaryGeneratorAction_h
#define ZDCPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"

class G4ParticleGun;
class G4GeneralParticleSource;
class G4GenericMessenger;
class G4Event;
class G4ParticleDefinition;

namespace ZDC
{

/// The primary generator action class with particle gum.
///
/// It defines a single particle which hits the calorimeter
/// perpendicular to the input face. The type of the particle
/// can be changed via the G4 build-in commands of G4ParticleGun class
/// (see the macros provided with this example).

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override;

    void GeneratePrimaries(G4Event* event) override;

  private:
//    G4ParticleGun* fParticleGun = nullptr;  // G4 particle gun
    G4GeneralParticleSource* fParticleGun;

};

}  // namespace ZDC

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
