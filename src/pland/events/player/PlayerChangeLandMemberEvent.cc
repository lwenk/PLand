#include "PlayerChangeLandMemberEvent.h"
#include "pland/events/Helper.h"

namespace land ::event {


mce::UUID IPlayerChangeLandMemberEvent::target() const { return mTarget; }

bool IPlayerChangeLandMemberEvent::isAdd() const { return mIsAdd; }

IMPLEMENT_EVENT_EMITTER(PlayerChangeLandMemberBeforeEvent)
IMPLEMENT_EVENT_EMITTER(PlayerChangeLandMemberAfterEvent)


} // namespace land::event