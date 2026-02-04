#pragma once
#include "pland/Global.h"
#include "pland/events/LandEventMixin.h"

#include <ll/api/event/Cancellable.h>
#include <ll/api/event/player/PlayerEvent.h>

#include <utility>

namespace land {
struct LandResizeSettlement;
class LandAABB;
class Land;
} // namespace land

namespace land {
namespace event {

class PlayerChangeLandRangeEvent final : public ll::event::Cancellable<LandEventMixin<ll::event::PlayerEvent>> {
    LandAABB const&             mNewRange;
    LandResizeSettlement const& mResizeSettlement;

public:
    explicit PlayerChangeLandRangeEvent(
        Player&                     player,
        std::shared_ptr<Land>       land,
        LandAABB const&             newRange,
        LandResizeSettlement const& resizeSettlement
    )
    : Cancellable(std::move(land), player),
      mNewRange(newRange),
      mResizeSettlement(resizeSettlement) {}

    LDNDAPI LandAABB const& newRange() const;

    LDNDAPI LandResizeSettlement const& resizeSettlement() const;
};

} // namespace event
} // namespace land
