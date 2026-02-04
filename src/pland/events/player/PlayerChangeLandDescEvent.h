#pragma once
#include "pland/Global.h"

#include <utility>

#include "ll/api/event/Cancellable.h"
#include "ll/api/event/player/PlayerEvent.h"


namespace land {
class Land;
namespace event {


class IPlayerChangeLandDescEvent : public ll::event::PlayerEvent {
    std::shared_ptr<Land> mLand;
    std::string&          mDesc;

public:
    explicit IPlayerChangeLandDescEvent(Player& player, std::shared_ptr<Land> land, std::string& desc)
    : PlayerEvent(player),
      mLand(std::move(land)),
      mDesc(desc) {}

    LDNDAPI std::shared_ptr<Land> land() const;

    LDNDAPI std::string& description() const;
};

class PlayerChangeLandDescBeforeEvent final : public ll::event::Cancellable<IPlayerChangeLandDescEvent> {
public:
    using ll::event::Cancellable<IPlayerChangeLandDescEvent>::Cancellable;
};
class PlayerChangeLandDescAfterEvent final : public IPlayerChangeLandDescEvent {
public:
    using IPlayerChangeLandDescEvent::IPlayerChangeLandDescEvent;
};

} // namespace event
} // namespace land