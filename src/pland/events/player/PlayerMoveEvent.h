#pragma once
#include "pland/Global.h"

#include <ll/api/event/player/PlayerEvent.h>

namespace land {
namespace event {

class PlayerMoveEvent : public ll::event::PlayerEvent {
    LandID mLandID;

public:
    explicit PlayerMoveEvent(Player& player, LandID landID) : PlayerEvent(player), mLandID(landID) {}

    LDNDAPI LandID landId() const;
};

class PlayerEnterLandEvent final : public PlayerMoveEvent {
    using PlayerMoveEvent::PlayerMoveEvent;
};
class PlayerLeaveLandEvent final : public PlayerMoveEvent {
    using PlayerMoveEvent::PlayerMoveEvent;
};


} // namespace event
} // namespace land
