#include "pland/land/LandEvent.h"
#include "ll/api/event/Emitter.h"
#include "mc/world/actor/player/Player.h"
#include "pland/land/Land.h"
#include "pland/selector/ISelector.h"


namespace land {


Player& PlayerEnterLandEvent::getPlayer() const { return mPlayer; }
LandID  PlayerEnterLandEvent::getLandID() const { return mLandID; }
Player& PlayerLeaveLandEvent::getPlayer() const { return mPlayer; }
LandID  PlayerLeaveLandEvent::getLandID() const { return mLandID; }


Player&    PlayerDeleteLandBeforeEvent::getPlayer() const { return mPlayer; }
LandID     PlayerDeleteLandBeforeEvent::getLandID() const { return mLandID; }
int const& PlayerDeleteLandBeforeEvent::getRefundPrice() const { return mRefundPrice; }
Player&    PlayerDeleteLandAfterEvent::getPlayer() const { return mPlayer; }
LandID     PlayerDeleteLandAfterEvent::getLandID() const { return mLandID; }


Player&          LandMemberChangeBeforeEvent::getPlayer() const { return mPlayer; }
mce::UUID const& LandMemberChangeBeforeEvent::getTargetPlayer() const { return mTargetPlayer; }
LandID           LandMemberChangeBeforeEvent::getLandID() const { return mLandID; }
bool             LandMemberChangeBeforeEvent::isAdd() const { return mIsAdd; }
Player&          LandMemberChangeAfterEvent::getPlayer() const { return mPlayer; }
mce::UUID const& LandMemberChangeAfterEvent::getTargetPlayer() const { return mTargetPlayer; }
LandID           LandMemberChangeAfterEvent::getLandID() const { return mLandID; }
bool             LandMemberChangeAfterEvent::isAdd() const { return mIsAdd; }


Player&          LandOwnerChangeBeforeEvent::getPlayer() const { return mPlayer; }
mce::UUID const& LandOwnerChangeBeforeEvent::getNewOwner() const { return mNewOwner; }
LandID           LandOwnerChangeBeforeEvent::getLandID() const { return mLandID; }
Player&          LandOwnerChangeAfterEvent::getPlayer() const { return mPlayer; }
mce::UUID const& LandOwnerChangeAfterEvent::getNewOwner() const { return mNewOwner; }
LandID           LandOwnerChangeAfterEvent::getLandID() const { return mLandID; }


// EventEmitter
#define IMPLEMENT_EVENT_EMITTER(EventName)                                                                             \
    static std::unique_ptr<ll::event::EmitterBase> emitterFactory##EventName();                                        \
    class EventName##Emitter : public ll::event::Emitter<emitterFactory##EventName, EventName> {};                     \
    static std::unique_ptr<ll::event::EmitterBase> emitterFactory##EventName() {                                       \
        return std::make_unique<EventName##Emitter>();                                                                 \
    }

IMPLEMENT_EVENT_EMITTER(PlayerEnterLandEvent)
IMPLEMENT_EVENT_EMITTER(PlayerLeaveLandEvent)
IMPLEMENT_EVENT_EMITTER(PlayerDeleteLandBeforeEvent)
IMPLEMENT_EVENT_EMITTER(PlayerDeleteLandAfterEvent)
IMPLEMENT_EVENT_EMITTER(LandMemberChangeBeforeEvent)
IMPLEMENT_EVENT_EMITTER(LandMemberChangeAfterEvent)
IMPLEMENT_EVENT_EMITTER(LandOwnerChangeBeforeEvent)
IMPLEMENT_EVENT_EMITTER(LandOwnerChangeAfterEvent)

} // namespace land
