#include "PlayerRequestBuyLandEvent.h"

#include "pland/events/Helper.h"

namespace land {
namespace event {


int64_t& PlayerRequestBuyLandEvent::payMoney() const { return mPayMoney; }
LandType PlayerRequestBuyLandEvent::landType() const { return mLandType; }

IMPLEMENT_EVENT_EMITTER(PlayerRequestBuyLandEvent);

} // namespace event
} // namespace land