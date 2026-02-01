#include "LandResizedEvent.h"

#include "pland/events/Helper.h"

namespace land {
namespace event {
std::shared_ptr<Land> const& LandResizedEvent::land() const { return mLand; }
LandAABB const&              LandResizedEvent::newRange() const { return mNewRange; }

IMPLEMENT_EVENT_EMITTER(LandResizedEvent);
} // namespace event
} // namespace land