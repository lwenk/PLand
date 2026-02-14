#include "PlayerChangeLandNameEvent.h"
#include "pland/events/Helper.h"


namespace land::event {


std::string& IPlayerChangeLandNameEvent::newName() const { return mNewName; }


IMPLEMENT_EVENT_EMITTER(PlayerChangeLandNameBeforeEvent)
IMPLEMENT_EVENT_EMITTER(PlayerChangeLandNameAfterEvent)


} // namespace land::event