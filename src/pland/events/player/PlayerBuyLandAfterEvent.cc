#include "PlayerBuyLandAfterEvent.h"

#include "pland/events/Helper.h"

namespace land {
namespace event {


PlayerBuyLandAfterEvent::PlayerBuyLandAfterEvent(Player& player, const std::shared_ptr<Land>& land, int64_t payMoney)
: PlayerEvent(player),
  mLand(land),
  mPayMoney(payMoney) {}
std::shared_ptr<Land> PlayerBuyLandAfterEvent::land() const { return mLand; }

int64_t PlayerBuyLandAfterEvent::payMoney() const { return mPayMoney; }


IMPLEMENT_EVENT_EMITTER(PlayerBuyLandAfterEvent)

} // namespace event
} // namespace land