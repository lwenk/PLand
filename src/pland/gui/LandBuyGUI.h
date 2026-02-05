#pragma once
#include "pland/Global.h"

class Player;

namespace land {

class OrdinaryLandCreateSelector;
class LandResizeSelector;
class SubLandCreateSelector;

class LandBuyGUI {
public:
    /**
     * @brief 购买领地(调用对应的impl函数)
     */
    LDAPI static void sendTo(Player& player);

    /**
     * @brief 购买普通领地
     */
    static void _impl(Player& player, OrdinaryLandCreateSelector* selector);

    /**
     * @brief 购买新的领地范围
     */
    static void _impl(Player& player, LandResizeSelector* selector);

    /**
     * @brief 购买子领地
     */
    static void _impl(Player& player, SubLandCreateSelector* selector);
};

} // namespace land
