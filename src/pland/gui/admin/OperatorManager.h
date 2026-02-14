#pragma once
#include "pland/Global.h"

class Player;
namespace mce {
class UUID;
}

namespace land {
class Land;
namespace gui {

/**
 * @brief 领地操作员GUI
 */
class OperatorManager {
public:
    OperatorManager() = delete;

    LDAPI static void sendMainMenu(Player& player);

    LDAPI static void sendLandSelectModeMenu(Player& player);
    LDAPI static void sendLandIdSearchForm(Player& player);
    LDAPI static void sendAdvancedLandPicker(Player& player, mce::UUID targetPlayer);
    LDAPI static void sendAdvancedLandPicker(Player& player, std::vector<std::shared_ptr<Land>> lands);
};


} // namespace gui
} // namespace land