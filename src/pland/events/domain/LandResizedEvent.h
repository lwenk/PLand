#pragma once
#include "pland/Global.h"

#include "pland/events/LandEventMixin.h"

#include <memory>

namespace land {
class LandAABB;
class Land;
namespace event {

class LandResizedEvent final : public LandEventMixin<ll::event::Event> {
    LandAABB const& mNewRange;

public:
    explicit LandResizedEvent(std::shared_ptr<Land> land, LandAABB const& newRange)
    : LandEventMixin(std::move(land)),
      mNewRange(newRange) {}

    LDNDAPI LandAABB const& newRange() const;
};

} // namespace event
} // namespace land
