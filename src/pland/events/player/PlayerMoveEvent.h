#pragma once
#include "pland/Global.h"

#include <ll/api/event/player/PlayerEvent.h>

namespace land {
namespace event {

class IPlayerMoveEvent : public ll::event::PlayerEvent {
    LandID mLandID;

public:
    explicit IPlayerMoveEvent(Player& player, LandID landID) : PlayerEvent(player), mLandID(landID) {}

    LDNDAPI LandID landId() const;
};

class PlayerEnterLandEvent final : public IPlayerMoveEvent {
    using IPlayerMoveEvent::IPlayerMoveEvent;
};
class PlayerLeaveLandEvent final : public IPlayerMoveEvent {
    using IPlayerMoveEvent::IPlayerMoveEvent;
};


} // namespace event
} // namespace land
