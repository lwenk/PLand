#pragma once
#include "pland/Global.h"

#include "LandUpdateBaseEvent.h"

#include <memory>

namespace land {
class LandAABB;
class Land;
} // namespace land

namespace land {
namespace event {

class LandResizedEvent final : public LandUpdateBaseEvent {
    LandAABB const& mNewRange;

public:
    explicit LandResizedEvent(std::shared_ptr<Land> land, LandAABB const& newRange)
    : LandUpdateBaseEvent(std::move(land)),
      mNewRange(newRange) {}

    LDNDAPI LandAABB const& newRange() const;
};

} // namespace event
} // namespace land
