#include "PlayerChangeLandRangeEvent.h"

#include "pland/events/Helper.h"

namespace land {
namespace event {


LandAABB const&             PlayerChangeLandRangeEvent::newRange() const { return mNewRange; }
LandResizeSettlement const& PlayerChangeLandRangeEvent::resizeSettlement() const { return mResizeSettlement; }

IMPLEMENT_EVENT_EMITTER(PlayerChangeLandRangeEvent)

} // namespace event
} // namespace land