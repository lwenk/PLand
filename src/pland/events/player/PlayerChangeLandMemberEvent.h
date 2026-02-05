#pragma once
#include "ll/api/event/Cancellable.h"
#include <utility>

#include "ll/api/event/player/PlayerEvent.h"

#include "pland/events/LandEventMixin.h"

namespace land::event {


class IPlayerChangeLandMemberEvent : public LandEventMixin<ll::event::PlayerEvent> {
    mce::UUID mTarget;
    bool      mIsAdd;

public:
    explicit IPlayerChangeLandMemberEvent(Player& player, std::shared_ptr<Land> land, mce::UUID target, bool isAdd)
    : LandEventMixin(std::move(land), player),
      mTarget(target),
      mIsAdd(isAdd) {}

    mce::UUID target() const;

    bool isAdd() const;
};

class PlayerChangeLandMemberBeforeEvent final : public ll::event::Cancellable<IPlayerChangeLandMemberEvent> {
public:
    using ll::event::Cancellable<IPlayerChangeLandMemberEvent>::Cancellable;
};
class PlayerChangeLandMemberAfterEvent final : public IPlayerChangeLandMemberEvent {
public:
    using IPlayerChangeLandMemberEvent::IPlayerChangeLandMemberEvent;
};

} // namespace land::event