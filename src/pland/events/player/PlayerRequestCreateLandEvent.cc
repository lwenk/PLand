#include "PlayerRequestCreateLandEvent.h"

#include "pland/events/Helper.h"


namespace land::event {


LandType PlayerRequestCreateLandEvent::type() const { return mType; }


IMPLEMENT_EVENT_EMITTER(PlayerRequestCreateLandEvent);


} // namespace land::event