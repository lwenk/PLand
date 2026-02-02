#include "LandUpdateBaseEvent.h"

namespace land {
namespace event {


LandUpdateBaseEvent::LandUpdateBaseEvent(std::shared_ptr<Land> land) : mLand(std::move(land)) {}
std::shared_ptr<Land> LandUpdateBaseEvent::land() const { return mLand; }


} // namespace event
} // namespace land