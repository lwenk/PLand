#pragma once
#include "pland/Global.h"
#include "pland/events/LandEventMixin.h"

#include <ll/api/event/Cancellable.h>
#include <ll/api/event/player/PlayerEvent.h>

namespace land {
class Land;
}
namespace land {
namespace event {

class IPlayerDeleteLandEvent : public LandEventMixin<ll::event::PlayerEvent> {
public:
    explicit IPlayerDeleteLandEvent(Player& player, std::shared_ptr<Land> land)
    : LandEventMixin(std::move(land), player) {}
};

class PlayerDeleteLandBeforeEvent final : public ll::event::Cancellable<IPlayerDeleteLandEvent> {
    using Cancellable::Cancellable;
};

class PlayerDeleteLandAfterEvent final : public IPlayerDeleteLandEvent {
    using IPlayerDeleteLandEvent::IPlayerDeleteLandEvent;
};

} // namespace event
} // namespace land
