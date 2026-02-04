#pragma once
#include "ll/api/event/Cancellable.h"
#include "ll/api/event/player/PlayerEvent.h"
#include "pland/Global.h"
#include <memory>
#include <utility>


namespace land {
class Land;
namespace event {


class IPlayerChangeLandNameEvent : public ll::event::PlayerEvent {
    std::shared_ptr<Land> mLand;
    std::string&          mNewName;

public:
    explicit IPlayerChangeLandNameEvent(Player& player, std::shared_ptr<Land> land, std::string& newName)
    : PlayerEvent{player},
      mLand{std::move(land)},
      mNewName{newName} {}

    LDNDAPI std::shared_ptr<Land> land() const;

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