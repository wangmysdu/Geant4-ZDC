#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"

namespace ZDC {

class EventAction; //必须包含在ZDC namespace中

class TrackingAction : public G4UserTrackingAction
{
  public:
    TrackingAction(EventAction* eventAction);
    virtual ~TrackingAction() {};

    virtual void PreUserTrackingAction(const G4Track*);
    virtual void PostUserTrackingAction(const G4Track*);

    private:
      EventAction*  fEventAction;
};

}

#endif