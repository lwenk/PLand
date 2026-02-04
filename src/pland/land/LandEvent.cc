#include "pland/land/LandEvent.h"
#include "ll/api/event/Emitter.h"
#include "mc/world/actor/player/Player.h"
#include "pland/land/Land.h"
#include "pland/selector/ISelector.h"


namespace land {

Player&          LandMemberChangeBeforeEvent::getPlayer() const { return mPlayer; }
mce::UUID const& LandMemberChangeBeforeEvent::getTargetPlayer() const { return mTargetPlayer; }
LandID           LandMemberChangeBeforeEvent::getLandID() const { return mLandID; }
bool             LandMemberChangeBeforeEvent::isAdd() const { return mIsAdd; }
Player&          LandMemberChangeAfterEvent::getPlayer() const { return mPlayer; }
mce::UUID const& LandMemberChangeAfterEvent::getTargetPlayer() const { return mTargetPlayer; }
LandID           LandMemberChangeAfterEvent::getLandID() const { return mLandID; }
bool             LandMemberChangeAfterEvent::isAdd() const { return mIsAdd; }


// EventEmitter
#define IMPLEMENT_EVENT_EMITTER(EventName)                                                                             \
    static std::unique_ptr<ll::event::EmitterBase> emitterFactory##EventName();                                        \
    class EventName##Emitter : public ll::event::Emitter<emitterFactory##EventName, EventName> {};                     \
    static std::unique_ptr<ll::event::EmitterBase> emitterFactory##EventName() {                                       \
        return std::make_unique<EventName##Emitter>();                                                                 \
    }

IMPLEMENT_EVENT_EMITTER(LandMemberChangeBeforeEvent)
IMPLEMENT_EVENT_EMITTER(LandMemberChangeAfterEvent)

} // namespace land
