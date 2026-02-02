#pragma once
#include "pland/Global.h"
#include "pland/land/LandType.h"

#include <ll/api/event/player/PlayerEvent.h>

namespace land::event {


class PlayerRequestCreateLandEvent final : public ll::event::PlayerEvent {
public:
    explicit PlayerRequestCreateLandEvent(Player& player, LandType type);

    LDAPI LandType type() const;

private:
    LandType mType;
};


} // namespace land::event