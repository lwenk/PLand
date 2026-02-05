#pragma once
#include "pland/events/LandEventMixin.h"

#include "ll/api/event/Cancellable.h"
#include "ll/api/event/player/PlayerEvent.h"

#include <memory>
#include <utility>

namespace land::event {


class IPlayerRequestChangeLandRangeEvent : public LandEventMixin<ll::event::PlayerEvent> {
public:
    explicit IPlayerRequestChangeLandRangeEvent(Player& player, std::shared_ptr<Land> land)
    : LandEventMixin(std::move(land), player) {}
};


class PlayerRequestChangeLandRangeBeforeEvent final
: public ll::event::Cancellable<IPlayerRequestChangeLandRangeEvent> {
public:
    using ll::event::Cancellable<IPlayerRequestChangeLandRangeEvent>::Cancellable;
};

class PlayerRequestChangeLandRangeAfterEvent final : public IPlayerRequestChangeLandRangeEvent {
public:
    using IPlayerRequestChangeLandRangeEvent::IPlayerRequestChangeLandRangeEvent;
};


} // namespace land::event