#include "pland/events/player/PlayerChangeLandDescEvent.h"
#include "pland/events/Helper.h"


namespace land::event {


std::string& IPlayerChangeLandDescEvent::description() const { return mDesc; }

IMPLEMENT_EVENT_EMITTER(PlayerChangeLandDescBeforeEvent)
IMPLEMENT_EVENT_EMITTER(PlayerChangeLandDescAfterEvent)

} // namespace land::event