#pragma once
#include <ll/api/event/Event.h>
#include <ll/api/event/player/PlayerEvent.h>

namespace land::event {


class PlayerRequestCreateLandEvent final : public ll::event::PlayerEvent {
public:
    enum class Type { Ordinary, Sub };

    explicit PlayerRequestCreateLandEvent(Player& player, Type type);

    Type type() const;

private:
    Type mType;
};


} // namespace land::event