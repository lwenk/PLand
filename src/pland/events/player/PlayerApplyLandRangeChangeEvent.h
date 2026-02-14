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

class IPlayerApplyLandRangeChangeEvent : public LandEventMixin<ll::event::PlayerEvent> {
    LandAABB const&             mNewRange;
    LandResizeSettlement const& mResizeSettlement;

public:
    explicit IPlayerApplyLandRangeChangeEvent(
        Player&                     player,
        std::shared_ptr<Land>       land,
        LandAABB const&             newRange,
        LandResizeSettlement const& resizeSettlement
    )
    : LandEventMixin(std::move(land), player),
      mNewRange(newRange),
      mResizeSettlement(resizeSettlement) {}

    LDNDAPI LandAABB const& newRange() const;

    LDNDAPI LandResizeSettlement const& resizeSettlement() const;
};


class PlayerApplyLandRangeChangeBeforeEvent final : public ll::event::Cancellable<IPlayerApplyLandRangeChangeEvent> {
public:
    using ll::event::Cancellable<IPlayerApplyLandRangeChangeEvent>::Cancellable;
};
class PlayerApplyLandRangeChangeAfterEvent final : public IPlayerApplyLandRangeChangeEvent {
public:
    using IPlayerApplyLandRangeChangeEvent::IPlayerApplyLandRangeChangeEvent;
};

} // namespace event
} // namespace land
