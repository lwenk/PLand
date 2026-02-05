#include "PlayerSettingGUI.h"

#include "pland/PLand.h"
#include "pland/land/repo/LandRegistry.h"
#include "pland/utils/FeedbackUtils.h"

#include "mc/world/actor/player/Player.h"

#include "ll/api/form/CustomForm.h"

namespace land {
using namespace ll::form;

void PlayerSettingGUI::sendTo(Player& player) {
    auto setting = PLand::getInstance().getLandRegistry().getPlayerSettings(player.getUuid());

    auto       localeCode = player.getLocaleCode();
    CustomForm fm(("[PLand] | 玩家设置"_trl(localeCode)));

    fm.appendToggle("showEnterLandTitle", "是否显示进入领地提示"_trl(localeCode), setting->showEnterLandTitle);
    fm.appendToggle("showBottomContinuedTip", "是否持续显示底部提示"_trl(localeCode), setting->showBottomContinuedTip);

    fm.sendTo(player, [setting](Player& pl, CustomFormResult res, FormCancelReason) {
        if (!res) {
            return;
        }

        setting->showEnterLandTitle     = std::get<uint64_t>(res->at("showEnterLandTitle"));
        setting->showBottomContinuedTip = std::get<uint64_t>(res->at("showBottomContinuedTip"));

        feedback_utils::sendText(pl, "设置已保存"_trl(pl.getLocaleCode()));
    });
}


} // namespace land