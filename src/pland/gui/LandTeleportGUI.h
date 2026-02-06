#pragma once
#include "pland/Global.h"

class Player;

namespace land {

class Land;

namespace gui {

class LandTeleportGUI {
public:
    LandTeleportGUI() = delete;

    LDAPI static void sendTo(Player& player); // sendTo -> impl

    LDAPI static void impl(Player& player, std::shared_ptr<Land> land);
};

} // namespace gui
} // namespace land