#pragma once
#include "pland/Global.h"
#include "pland/selector/land/LandResizeSelector.h"
#include "pland/selector/land/OrdinaryLandCreateSelector.h"
#include "pland/selector/land/SubLandCreateSelector.h"



namespace land {

class LandBuyGUI {
public:
    /**
     * @brief 购买领地(调用对应的impl函数)
     */
    LDAPI static void impl(Player& player);

    /**
     * @brief 购买普通领地
     */
    LDAPI static void impl(Player& player, OrdinaryLandCreateSelector* selector);

    /**
     * @brief 购买新的领地范围
     */
    LDAPI static void impl(Player& player, LandResizeSelector* selector);

    /**
     * @brief 购买子领地
     */
    LDAPI static void impl(Player& player, SubLandCreateSelector* selector);
};

} // namespace land
