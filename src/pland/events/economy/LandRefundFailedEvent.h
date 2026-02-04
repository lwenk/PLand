#pragma once
#include "pland/Global.h"
#include "pland/events/LandEventMixin.h"


#include <ll/api/event/Event.h>

namespace mce {
class UUID;
}

namespace land {
class Land;
namespace event {

class LandRefundFailedEvent final : public LandEventMixin<ll::event::Event> {
    mce::UUID const& mTargetPlayer;
    int64_t          mRefundAmount{0};

public:
    explicit LandRefundFailedEvent(
        std::shared_ptr<Land> const& land,
        mce::UUID const&             targetPlayer,
        int64_t                      refundAmount
    )
    : LandEventMixin(land),
      mTargetPlayer(targetPlayer),
      mRefundAmount(refundAmount) {}

    LDNDAPI mce::UUID const& targetPlayer() const;

    LDNDAPI int64_t refundAmount() const;
};

} // namespace event
} // namespace land
