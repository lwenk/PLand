#pragma once
#include "pland/Global.h"
#include "pland/events/LandEventMixin.h"

#include "ll/api/event/Cancellable.h"
#include "ll/api/event/player/PlayerEvent.h"

#include <memory>
#include <utility>


namespace land {
class Land;
namespace event {


class IPlayerChangeLandNameEvent : public LandEventMixin<ll::event::PlayerEvent> {
    std::string& mNewName;

public:
    explicit IPlayerChangeLandNameEvent(Player& player, std::shared_ptr<Land> land, std::string& newName)
    : LandEventMixin{std::move(land), player},
      mNewName{newName} {}

    LDNDAPI std::string& newName() const;
};


class PlayerChangeLandNameBeforeEvent final : public ll::event::Cancellable<IPlayerChangeLandNameEvent> {
public:
    using ll::event::Cancellable<IPlayerChangeLandNameEvent>::Cancellable;
};

class PlayerChangeLandNameAfterEvent final : public IPlayerChangeLandNameEvent {
public:
    using IPlayerChangeLandNameEvent::IPlayerChangeLandNameEvent;
};


} // namespace event
} // namespace land