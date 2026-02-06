#include "LandTeleportGUI.h"
#include "pland/PLand.h"
#include "pland/gui/LandMainMenuGUI.h"
#include "pland/gui/common/AdvancedLandPicker.h"
#include "pland/land/Land.h"
#include "pland/land/internal/SafeTeleport.h"
#include "pland/land/repo/LandRegistry.h"
#include "pland/utils/McUtils.h"
#include "utils/BackUtils.h"


namespace land {


void LandTeleportGUI::sendTo(Player& player) {
    gui::AdvancedLandPicker::sendTo(
        player,
        PLand::getInstance().getLandRegistry().getLands(player.getUuid(), true),
        impl,
        gui::back_utils::wrapCallback<sendTo>()
    );
}

void LandTeleportGUI::impl(Player& player, std::shared_ptr<Land> land) {
    auto const& tpPos = land->getTeleportPos();
    // TODO: 改进未设置语义，迁移到 std::optional<T>
    if (tpPos.isZero() || !land->getAABB().hasPos(tpPos.as<Vec3>())) {
        if (!tpPos.isZero()) {
            land->setTeleportPos(LandPos::make(0, 0, 0));
        }
        PLand::getInstance().getSafeTeleport().launchTask(
            player,
            land->getAABB().getMin().as(),
            land->getDimensionId()
        );
        return;
    }
    auto v3 = tpPos.as<Vec3>();
    player.teleport(v3, land->getDimensionId());
}


} // namespace land