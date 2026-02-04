#include "PlayerBuyLandEvent.h"

#include "pland/events/Helper.h"

namespace land {
namespace event {


int64_t& PlayerBuyLandBeforeEvent::payMoney() const { return mPayMoney; }
LandType PlayerBuyLandBeforeEvent::landType() const { return mLandType; }

IMPLEMENT_EVENT_EMITTER(PlayerBuyLandBeforeEvent);


int64_t PlayerBuyLandAfterEvent::payMoney() const { return mPayMoney; }

IMPLEMENT_EVENT_EMITTER(PlayerBuyLandAfterEvent)


} // namespace event
} // namespace land