#pragma once
#include "ll/api/event/Event.h"

#include "mc/platform/UUID.h"
#include "pland/events/LandEventMixin.h"
#include <memory>
#include <utility>


namespace land ::event {


class MemberChangedEvent final : public LandEventMixin<ll::event::Event> {
    mce::UUID mTarget;
    bool      mIsAdd; // true if added, false if removed

public:
    explicit MemberChangedEvent(std::shared_ptr<Land> land, mce::UUID target, bool isAdd)
    : LandEventMixin(std::move(land)),
      mTarget(target),
      mIsAdd(isAdd) {}

    mce::UUID target() const;

    bool isAdd() const;
};


} // namespace land::event