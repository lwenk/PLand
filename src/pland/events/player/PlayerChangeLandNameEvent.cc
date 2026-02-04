#include "PlayerChangeLandNameEvent.h"
#include "pland/events/Helper.h"

#include <memory>


namespace land::event {


std::shared_ptr<Land> IPlayerChangeLandNameEvent::land() const { return mLand; }

std::string& IPlayerChangeLandNameEvent::newName() const { return mNewName; }


IMPLEMENT_EVENT_EMITTER(PlayerChangeLandNameBeforeEvent)
IMPLEMENT_EVENT_EMITTER(PlayerChangeLandNameAfterEvent)


} // namespace land::event