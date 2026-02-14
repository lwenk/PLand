#pragma once
#include "pland/Global.h"

#include "ll/api/event/Event.h"


namespace land {
struct Config;
}

namespace land::events::inline infra {


class ConfigReloadEvent final : public ll::event::Event {
    Config& mConfig;

public:
    explicit ConfigReloadEvent(Config& config) : mConfig(config) {}

    LDNDAPI Config& config() const;
};


} // namespace land::events::inline infra