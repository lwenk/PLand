#pragma once
#include "pland/Global.h"

#include <ll/api/event/Cancellable.h>
#include <ll/api/event/player/PlayerEvent.h>

namespace land {
class Land;
}
namespace land {
namespace event {

class PlayerDeleteLandEvent : public ll::event::Cancellable<ll::event::PlayerEvent> {
    std::shared_ptr<Land> mLand;

public:
    explicit PlayerDeleteLandEvent(Player& player, std::shared_ptr<Land> land)
    : Cancellable(player),
      mLand(std::move(land)) {}

    LDNDAPI std::shared_ptr<Land> land() const;
};

class PlayerDeleteLandBeforeEvent final : public PlayerDeleteLandEvent {
    using PlayerDeleteLandEvent::PlayerDeleteLandEvent;
};

class PlayerDeleteLandAfterEvent final : public PlayerDeleteLandEvent {
    using PlayerDeleteLandEvent::PlayerDeleteLandEvent;
};

} // namespace event
} // namespace land
