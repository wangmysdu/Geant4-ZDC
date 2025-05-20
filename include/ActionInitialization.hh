#ifndef ZDCActionInitialization_h
#define ZDCActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

namespace ZDC
{

/// Action initialization class.

class ActionInitialization : public G4VUserActionInitialization
{
public:
    ActionInitialization() = default;
    ~ActionInitialization() override = default;

    void BuildForMaster() const override;
    void Build() const override;
};

}  // namespace ZDC

#endif
