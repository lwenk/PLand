#include "pland/events/player/PlayerChangeLandDescEvent.h"
#include "pland/events/Helper.h"

#include <memory>


namespace land::event {


std::shared_ptr<Land> IPlayerChangeLandDescEvent::land() const { return mLand; }

std::string& IPlayerChangeLandDescEvent::description() const { return mDesc; }

IMPLEMENT_EVENT_EMITTER(PlayerChangeLandDescBeforeEvent)
IMPLEMENT_EVENT_EMITTER(PlayerChangeLandDescAfterEvent)

} // namespace land::event