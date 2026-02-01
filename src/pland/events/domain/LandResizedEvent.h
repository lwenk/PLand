#pragma once
#include "pland/Global.h"


#include <ll/api/event/Event.h>
#include <memory>

namespace land {
class LandAABB;
class Land;
} // namespace land

namespace land {
namespace event {

class LandResizedEvent final : public ll::event::Event {
    std::shared_ptr<Land> mLand;
    LandAABB const&       mNewRange;

public:
    explicit LandResizedEvent(std::shared_ptr<Land> land, LandAABB const& newRange)
    : mLand(land),
      mNewRange(newRange) {}

    LDNDAPI std::shared_ptr<Land> const& land() const;

    LDNDAPI LandAABB const& newRange() const;
};

} // namespace event
} // namespace land
