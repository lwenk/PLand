#include "PlayerDeleteLandEvent.h"

#include "pland/events/Helper.h"

namespace land {
namespace event {

IMPLEMENT_EVENT_EMITTER(PlayerDeleteLandBeforeEvent)
IMPLEMENT_EVENT_EMITTER(PlayerDeleteLandAfterEvent)

} // namespace event
} // namespace land