#include "PlayerApplyLandRangeChangeEvent.h"

#include "pland/events/Helper.h"

namespace land {
namespace event {


LandAABB const&             IPlayerApplyLandRangeChangeEvent::newRange() const { return mNewRange; }
LandResizeSettlement const& IPlayerApplyLandRangeChangeEvent::resizeSettlement() const { return mResizeSettlement; }

IMPLEMENT_EVENT_EMITTER(PlayerApplyLandRangeChangeBeforeEvent)
IMPLEMENT_EVENT_EMITTER(PlayerApplyLandRangeChangeAfterEvent)

} // namespace event
} // namespace land