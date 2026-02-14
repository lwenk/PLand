#include "LandResizedEvent.h"

#include "pland/events/Helper.h"

namespace land ::event {

LandAABB const& LandResizedEvent::newRange() const { return mNewRange; }

IMPLEMENT_EVENT_EMITTER(LandResizedEvent);

} // namespace land::event