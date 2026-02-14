#include "OwnerChangedEvent.h"
#include "pland/events/Helper.h"

#include "mc/platform/UUID.h"

namespace land::event {


mce::UUID OwnerChangedEvent::oldOwner() const { return mOldOwner; }
mce::UUID OwnerChangedEvent::newOwner() const { return mNewOwner; }

IMPLEMENT_EVENT_EMITTER(OwnerChangedEvent)

} // namespace land::event