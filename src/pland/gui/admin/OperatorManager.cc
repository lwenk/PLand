#include "OperatorManager.h"

#include "LandOwnerPicker.h"
#include "pland/PLand.h"
#include "pland/gui/LandManagerGUI.h"
#include "pland/gui/PermTableEditor.h"
#include "pland/gui/common/AdvancedLandPicker.h"
#include "pland/land/Land.h"
#include "pland/land/LandTemplatePermTable.h"
#include "pland/land/repo/LandContext.h"
#include "pland/land/repo/LandRegistry.h"
#include "pland/utils/FeedbackUtils.h"

#include "ll/api/service/PlayerInfo.h"
#include "pland/gui/utils/BackUtils.h"

#include <ll/api/form/SimpleForm.h>

namespace land::gui {


void OperatorManager::sendMainMenu(Player& player) {
    auto localeCode = player.getLocaleCode();

    if (!PLand::getInstance().getLandRegistry().isOperator(player.getUuid())) {
        feedback_utils::sendErrorText(player, "无权限访问此表单"_trl(localeCode));
        return;
    }

    auto fm = ll::form::SimpleForm{};

    fm.setTitle("[PLand] | 领地管理"_trl(localeCode));
    fm.setContent("请选择您要进行的操作"_trl(localeCode));

    fm.appendButton("管理脚下领地"_trl(localeCode), "textures/ui/free_download", "path", [](Player& self) {
        auto lands = PLand::getInstance().getLandRegistry().getLandAt(self.getPosition(), self.getDimensionId());
        if (!lands) {
            feedback_utils::sendErrorText(self, "您当前所处位置没有领地"_trl(self.getLocaleCode()));
            return;
        }
        LandManagerGUI::sendMainMenu(self, lands);
    });
    fm.appendButton("管理玩家领地"_trl(localeCode), "textures/ui/FriendsIcon", "path", [](Player& self) {
        LandOwnerPicker::sendTo(self, static_cast<void (*)(Player&, mce::UUID)>(&sendAdvancedLandPicker), sendMainMenu);
    });
    fm.appendButton("管理指定领地"_trl(localeCode), "textures/ui/magnifyingGlass", "path", [](Player& self) {
        sendAdvancedLandPicker(self, PLand::getInstance().getLandRegistry().getLands());
    });
    fm.appendButton("编辑默认权限"_trl(localeCode), "textures/ui/icon_map", "path", [](Player& self) {
        gui::PermTableEditor::sendTo(
            self,
            PLand::getInstance().getLandRegistry().getLandTemplatePermTable().get(),
            [](Player& self, LandPermTable newTable) {
                PLand::getInstance().getLandRegistry().getLandTemplatePermTable().set(newTable);
                feedback_utils::sendText(self, "权限表已更新"_trl(self.getLocaleCode()));
            }
        );
    });

    fm.sendTo(player);
}

void OperatorManager::sendAdvancedLandPicker(Player& player, mce::UUID targetPlayer) {
    sendAdvancedLandPicker(player, PLand::getInstance().getLandRegistry().getLands(targetPlayer));
}

void OperatorManager::sendAdvancedLandPicker(Player& player, std::vector<std::shared_ptr<Land>> lands) {
    AdvancedLandPicker::sendTo(
        player,
        lands,
        [](Player& self, std::shared_ptr<Land> ptr) { LandManagerGUI::sendMainMenu(self, ptr); },
        back_utils::wrapCallback<sendMainMenu>()
    );
}


} // namespace land::gui