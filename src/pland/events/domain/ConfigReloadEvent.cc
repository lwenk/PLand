#include "ConfigReloadEvent.h"
#include "pland/events/Helper.h"

namespace land::events::inline infra {

Config& ConfigReloadEvent::config() const { return mConfig; }

IMPLEMENT_EVENT_EMITTER(ConfigReloadEvent)

} // namespace land::events::inline infra