#pragma once
#include "pland/Global.h"
#include "pland/events/LandEventMixin.h"

#include "mc/platform/UUID.h"

#include <memory>
#include <utility>


namespace land::event {


class OwnerChangedEvent final : public LandEventMixin<ll::event::Event> {
    mce::UUID mOldOwner;
    mce::UUID mNewOwner;

public:
    explicit OwnerChangedEvent(std::shared_ptr<Land> land, mce::UUID oldOwner, mce::UUID newOwner)
    : LandEventMixin(std::move(land)),
      mOldOwner(oldOwner),
      mNewOwner(newOwner) {}

    LDNDAPI mce::UUID oldOwner() const;

    LDNDAPI mce::UUID newOwner() const;
};


} // namespace land::event