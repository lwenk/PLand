#include "PlayerTransferLandEvent.h"
#include "pland/events/Helper.h"

#include "mc/platform/UUID.h"

namespace land::event {


mce::UUID IPlayerTransferLandEvent::newOwner() const { return mNewOwner; }

IMPLEMENT_EVENT_EMITTER(PlayerTransferLandBeforeEvent)
IMPLEMENT_EVENT_EMITTER(PlayerTransferLandAfterEvent)


} // namespace land::event
