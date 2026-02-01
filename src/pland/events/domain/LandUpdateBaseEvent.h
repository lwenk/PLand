#pragma once
#include "pland/Global.h"

#include <ll/api/event/Event.h>

#include <memory>

namespace land {
class Land;
namespace event {

class LandUpdateBaseEvent : public ll::event::Event {
    std::shared_ptr<Land> mLand;

public:
    explicit LandUpdateBaseEvent(std::shared_ptr<Land> land);

    LDNDAPI std::shared_ptr<Land> land() const;
};

} // namespace event
} // namespace land
