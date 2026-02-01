#include "PlayerRequestChangeLandRangeEvent.h"

#include "pland/events/Helper.h"

namespace land {
namespace event {


std::shared_ptr<Land>       PlayerRequestChangeLandRangeEvent::land() const { return mLand; }
LandAABB const&             PlayerRequestChangeLandRangeEvent::newRange() const { return mNewRange; }
LandResizeSettlement const& PlayerRequestChangeLandRangeEvent::resizeSettlement() const { return mResizeSettlement; }

IMPLEMENT_EVENT_EMITTER(PlayerRequestChangeLandRangeEvent)

} // namespace event
} // namespace land