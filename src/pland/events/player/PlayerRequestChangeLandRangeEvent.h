#pragma once
#include "pland/Global.h"


#include <ll/api/event/Cancellable.h>
#include <ll/api/event/player/PlayerEvent.h>

namespace land {
struct LandResizeSettlement;
class LandAABB;
class Land;
} // namespace land

namespace land {
namespace event {

class PlayerRequestChangeLandRangeEvent final : public ll::event::Cancellable<ll::event::PlayerEvent> {
    std::shared_ptr<Land>       mLand;
    LandAABB const&             mNewRange;
    LandResizeSettlement const& mResizeSettlement;

public:
    explicit PlayerRequestChangeLandRangeEvent(
        Player&                     player,
        std::shared_ptr<Land>       land,
        LandAABB const&             newRange,
        LandResizeSettlement const& resizeSettlement
    )
    : Cancellable(player),
      mLand(land),
      mNewRange(newRange),
      mResizeSettlement(resizeSettlement) {}

    LDNDAPI std::shared_ptr<Land> land() const;

    LDNDAPI LandAABB const& newRange() const;

    LDNDAPI LandResizeSettlement const& resizeSettlement() const;
};

} // namespace event
} // namespace land
