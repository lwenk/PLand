#pragma once
#include "ll/api/event/player/PlayerEvent.h"
#include "pland/Global.h"
#include "pland/land/LandType.h"

#include <ll/api/event/Cancellable.h>

namespace land {
namespace event {

class PlayerRequestBuyLandEvent final : public ll::event::Cancellable<ll::event::PlayerEvent> {
    int64_t& mPayMoney;
    LandType mLandType;

public:
    explicit PlayerRequestBuyLandEvent(Player& player, int64_t& payMoney, LandType landType)
    : Cancellable(player),
      mPayMoney(payMoney),
      mLandType(landType) {}

    LDNDAPI int64_t& payMoney() const;

    LDNDAPI LandType landType() const;
};

} // namespace event
} // namespace land
