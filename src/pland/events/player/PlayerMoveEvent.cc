#include "PlayerMoveEvent.h"

#include "pland/events/Helper.h"

namespace land {
namespace event {

LandID PlayerMoveEvent::landId() const { return mLandID; }

IMPLEMENT_EVENT_EMITTER(PlayerEnterLandEvent)
IMPLEMENT_EVENT_EMITTER(PlayerLeaveLandEvent)

} // namespace event
} // namespace land