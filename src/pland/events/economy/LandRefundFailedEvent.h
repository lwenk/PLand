#pragma once
#include "pland/Global.h"


#include <ll/api/event/Event.h>

namespace mce {
class UUID;
}

namespace land {
class Land;
namespace event {

class LandRefundFailedEvent final : public ll::event::Event {
    std::shared_ptr<Land> mLand;
    mce::UUID const&      mTargetPlayer;
    int64_t               mRefundAmount{0};

public:
    explicit LandRefundFailedEvent(
        std::shared_ptr<Land> const& land,
        mce::UUID const&             targetPlayer,
        int64_t                      refundAmount
    )
    : mLand(land),
      mTargetPlayer(targetPlayer),
      mRefundAmount(refundAmount) {}

    LDNDAPI mce::UUID const& targetPlayer() const;

    LDNDAPI int64_t refundAmount() const;

    LDNDAPI std::shared_ptr<Land> land() const;
};

} // namespace event
} // namespace land
