#pragma once
#include "pland/Global.h"

#include <functional>

class Player;

namespace land::gui {


struct OnlinePlayerPicker {
    OnlinePlayerPicker() = delete;

    using Callback = std::function<void(Player& self, Player& target)>;

    using BackTo = std::function<void(Player& self)>;

    LDAPI static void
    sendTo(Player& player, Callback const& callback, BackTo backTo = nullptr, bool includeSimulatedPlayers = false);
};


} // namespace land::gui