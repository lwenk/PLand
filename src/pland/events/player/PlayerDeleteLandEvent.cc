#include "PlayerDeleteLandEvent.h"

#include "pland/events/Helper.h"

namespace land {
namespace event {


std::shared_ptr<Land> PlayerDeleteLandEvent::land() const { return mLand; }

IMPLEMENT_EVENT_EMITTER(PlayerDeleteLandBeforeEvent)
IMPLEMENT_EVENT_EMITTER(PlayerDeleteLandAfterEvent)

} // namespace event
} // namespace land