#pragma once
#include "pland/Global.h"

class Player;

namespace land ::gui {


class PlayerSettingGUI {
public:
    PlayerSettingGUI() = delete;

    LDAPI static void sendTo(Player& player);
};


} // namespace land::gui