#include "pland/gui/LandManagerGUI.h"
#include "LandTeleportGUI.h"

#include "ll/api/form/CustomForm.h"
#include "ll/api/form/FormBase.h"
#include "ll/api/form/ModalForm.h"
#include "ll/api/form/SimpleForm.h"
#include "ll/api/service/PlayerInfo.h"

#include "mc/world/actor/player/Player.h"
#include "mc/world/level/Level.h"

#include "pland/PLand.h"
#include "pland/gui/CommonUtilGUI.h"
#include "pland/gui/common/EditLandPermTableUtilGUI.h"
#include "pland/gui/form/BackSimpleForm.h"
#include "pland/land/Config.h"
#include "pland/land/repo/LandRegistry.h"
#include "pland/service/LandHierarchyService.h"
#include "pland/service/LandManagementService.h"
#include "pland/service/LandPriceService.h"
#include "pland/service/ServiceLocator.h"
#include "pland/utils/FeedbackUtils.h"

#include <string>
#include <utility>
#include <vector>


using namespace ll::form;

namespace land {


void LandManagerGUI::sendMainMenu(Player& player, SharedLand land) {
    auto fm = SimpleForm{};
    fm.setTitle(("[PLand] | 领地管理 [{}]"_trf(player, land->getId())));

    auto& service = PLand::getInstance().getServiceLocator().getLandHierarchyService();

    std::string subContent;
    if (land->isParentLand()) {
        subContent = "下属子领地: {}"_trf(player, land->getSubLandIDs().size());
    } else if (land->isMixLand()) {
        subContent = "下属子领地: {}\n父领地ID: {}\n父领地名称: {}"_trf(
            player,
            land->getSubLandIDs().size(),
            service.getParent(land)->getId(),
            service.getParent(land)->getName()
        );
    } else {
        subContent = "父领地ID: {}\n父领地名称: {}"_trf(
            player,
            land->hasParentLand() ? (std::to_string(service.getParent(land)->getId())) : "null",
            land->hasParentLand() ? service.getParent(land)->getName() : "null"
        );
    }

    fm.setContent(
        "领地: {}\n类型: {}\n大小: {}x{}x{} = {}\n范围: {}\n\n{}"_trf(
            player,
            land->getName(),
            land->is3D() ? "3D" : "2D",
            land->getAABB().getBlockCountX(),
            land->getAABB().getBlockCountZ(),
            land->getAABB().getBlockCountY(),
            land->getAABB().getVolume(),
            land->getAABB().toString(),
            subContent
        )
    );


    fm.appendButton("编辑权限"_trf(player), "textures/ui/sidebar_icons/promotag", "path", [land](Player& pl) {
        sendEditLandPermGUI(pl, land);
    });
    fm.appendButton("修改成员"_trf(player), "textures/ui/FriendsIcon", "path", [land](Player& pl) {
        sendChangeMember(pl, land);
    });
    fm.appendButton("修改领地名称"_trf(player), "textures/ui/book_edit_default", "path", [land](Player& pl) {
        sendEditLandNameGUI(pl, land);
    });
    fm.appendButton("修改领地描述"_trf(player), "textures/ui/book_edit_default", "path", [land](Player& pl) {
        sendEditLandDescGUI(pl, land);
    });

    // 开启了领地传送功能，或者玩家是领地管理员
    if (Config::cfg.land.landTp || PLand::getInstance().getLandRegistry().isOperator(player.getUuid())) {
        fm.appendButton("传送到领地"_trf(player), "textures/ui/icon_recipe_nature", "path", [land](Player& pl) {
            LandTeleportGUI::impl(pl, land);
        });

        // 如果玩家在领地内，则显示设置传送点按钮
        if (land->getAABB().hasPos(player.getPosition())) {
            fm.appendButton("设置传送点"_trf(player), "textures/ui/Add-Ons_Nav_Icon36x36", "path", [land](Player& pl) {
                auto& service = PLand::getInstance().getServiceLocator().getLandManagementService();
                if (auto res = service.setLandTeleportPos(pl, land, pl.getPosition())) {
                    feedback_utils::notifySuccess(pl, "传送点已设置!"_trf(pl));
                } else {
                    feedback_utils::sendError(pl, res.error());
                }
            });
        }
    }

    fm.appendButton("领地过户"_trf(player), "textures/ui/sidebar_icons/my_characters", "path", [land](Player& pl) {
        sendTransferLandGUI(pl, land);
    });

    if (Config::ensureSubLandFeatureEnabled() && land->canCreateSubLand()) {
        fm.appendButton("创建子领地"_trf(player), "textures/ui/icon_recipe_nature", "path", [land](Player& pl) {
            sendCreateSubLandConfirm(pl, land);
        });
    }

    if (land->isOrdinaryLand()) {
        fm.appendButton("重新选区"_trf(player), "textures/ui/anvil_icon", "path", [land](Player& pl) {
            sendChangeRangeConfirm(pl, land);
        });
    }

    fm.appendButton("删除领地"_trf(player), "textures/ui/icon_trash", "path", [land](Player& pl) {
        showRemoveConfirm(pl, land);
    });

    fm.sendTo(player);
}

void LandManagerGUI::sendEditLandPermGUI(Player& player, SharedLand const& ptr) {
    EditLandPermTableUtilGUI::sendTo(player, ptr->getPermTable(), [ptr](Player& self, LandPermTable newTable) {
        ptr->setPermTable(newTable);
        feedback_utils::sendText(self, "权限表已更新"_trf(self));
    });
}


void LandManagerGUI::showRemoveConfirm(Player& player, SharedLand const& ptr) {
    switch (ptr->getType()) {
    case LandType::Ordinary:
    case LandType::Sub:
        confirmSimpleDelete(player, ptr);
        break;
    case LandType::Parent:
        confirmParentDelete(player, ptr);
        break;
    case LandType::Mix:
        confirmMixDelete(player, ptr);
        break;
    }
}

void LandManagerGUI::confirmSimpleDelete(Player& player, SharedLand const& ptr) {
    if (!ptr->isOrdinaryLand() && !ptr->isSubLand()) {
        return;
    }
    ModalForm(
        "[PLand] | 确认删除?"_trf(player),
        "您确定要删除领地 '{}' 吗?\n删除领地后，您将获得 {} 金币的退款。\n此操作不可逆,请谨慎操作!"_trf(
            player,
            ptr->getName(),
            PLand::getInstance().getServiceLocator().getLandPriceService().getRefundAmount(ptr)
        ),
        "确认"_trf(player),
        "返回"_trf(player)
    )
        .sendTo(player, [ptr](Player& pl, ModalFormResult const& res, FormCancelReason) {
            if (!res) {
                return;
            }
            if (!(bool)res.value()) {
                sendMainMenu(pl, ptr);
                return;
            }

            auto& service = PLand::getInstance().getServiceLocator().getLandManagementService();
            if (auto expected = service.deleteLand(pl, ptr, service::DeletePolicy::CurrentOnly)) {
                feedback_utils::notifySuccess(pl, "删除成功"_trf(pl));
            } else {
                feedback_utils::sendError(pl, expected.error());
            }
        });
}

void LandManagerGUI::confirmParentDelete(Player& player, SharedLand const& ptr) {
    auto fm = BackSimpleForm<>::make<LandManagerGUI::sendMainMenu>(ptr);
    fm.setTitle("[PLand] | 删除领地 & 父领地"_trf(player));
    fm.setContent(
        "您当前操作的的是父领地\n当前领地下有 {} 个子领地\n您确定要删除领地吗?"_trf(player, ptr->getSubLandIDs().size())
    );

    fm.appendButton("删除当前领地和子领地"_trf(player), [ptr](Player& pl) {
        auto expected = PLand::getInstance().getServiceLocator().getLandManagementService().deleteLand(
            pl,
            ptr,
            service::DeletePolicy::Recursive
        );
        if (expected) {
            feedback_utils::notifySuccess(pl, "删除成功"_trf(pl));
        } else {
            feedback_utils::sendError(pl, expected.error());
        }
    });
    fm.appendButton("删除当前领地并提升子领地为父领地"_trf(player), [ptr](Player& pl) {
        auto expected = PLand::getInstance().getServiceLocator().getLandManagementService().deleteLand(
            pl,
            ptr,
            service::DeletePolicy::PromoteChildren
        );
        if (expected) {
            feedback_utils::notifySuccess(pl, "删除成功"_trf(pl));
        } else {
            feedback_utils::sendError(pl, expected.error());
        }
    });

    fm.sendTo(player);
}

void LandManagerGUI::confirmMixDelete(Player& player, SharedLand const& ptr) {
    auto fm = BackSimpleForm<>::make<LandManagerGUI::sendMainMenu>(ptr);
    fm.setTitle("[PLand] | 删除领地 & 混合领地"_trf(player));
    fm.setContent(
        "您当前操作的的是混合领地\n当前领地下有 {} 个子领地\n您确定要删除领地吗?"_trf(
            player,
            ptr->getSubLandIDs().size()
        )
    );

    fm.appendButton("删除当前领地和子领地"_trf(player), [ptr](Player& pl) {
        auto expected = PLand::getInstance().getServiceLocator().getLandManagementService().deleteLand(
            pl,
            ptr,
            service::DeletePolicy::Recursive
        );
        if (expected) {
            feedback_utils::notifySuccess(pl, "删除成功"_trf(pl));
        } else {
            feedback_utils::sendError(pl, expected.error());
        }
    });
    fm.appendButton("删除当前领地并移交子领地给父领地"_trf(player), [ptr](Player& pl) {
        auto expected = PLand::getInstance().getServiceLocator().getLandManagementService().deleteLand(
            pl,
            ptr,
            service::DeletePolicy::TransferChildren
        );
        if (expected) {
            feedback_utils::notifySuccess(pl, "删除成功"_trf(pl));
        } else {
            feedback_utils::sendError(pl, expected.error());
        }
    });

    fm.sendTo(player);
}

void LandManagerGUI::sendEditLandNameGUI(Player& player, SharedLand const& ptr) {
    EditStringUtilGUI::sendTo(
        player,
        "修改领地名称"_trf(player),
        "请输入新的领地名称"_trf(player),
        ptr->getName(),
        [ptr](Player& pl, std::string result) {
            auto& service = PLand::getInstance().getServiceLocator().getLandManagementService();
            if (auto expected = service.setLandName(pl, ptr, result)) {
                feedback_utils::sendText(pl, "领地名称已更新!"_trf(pl, result));
            } else {
                feedback_utils::sendError(pl, expected.error());
            }
        }
    );
}
void LandManagerGUI::sendEditLandDescGUI(Player& player, SharedLand const& ptr) {
    EditStringUtilGUI::sendTo(
        player,
        "修改领地描述"_trf(player),
        "请输入新的领地描述"_trf(player),
        ptr->getDescribe(),
        [ptr](Player& pl, std::string result) {
            auto& service = PLand::getInstance().getServiceLocator().getLandManagementService();
            if (auto expected = service.setLandDescription(pl, ptr, std::move(result))) {
                feedback_utils::sendText(pl, "领地描述已更新!"_trf(pl));
            } else {
                feedback_utils::sendError(pl, expected.error());
            }
        }
    );
}
void LandManagerGUI::sendTransferLandGUI(Player& player, SharedLand const& ptr) {
    auto fm = BackSimpleForm<>::make<LandManagerGUI::sendMainMenu>(ptr);
    fm.setTitle("[PLand] | 转让领地"_trf(player));

    fm.appendButton(
        "转让给在线玩家"_trf(player),
        "textures/ui/sidebar_icons/my_characters",
        "path",
        [ptr](Player& self) { _sendTransferLandToOnlinePlayer(self, ptr); }
    );

    fm.appendButton(
        "转让给离线玩家"_trf(player),
        "textures/ui/sidebar_icons/my_characters",
        "path",
        [ptr](Player& self) { _sendTransferLandToOfflinePlayer(self, ptr); }
    );

    fm.sendTo(player);
}
void LandManagerGUI::_sendTransferLandToOnlinePlayer(Player& player, const SharedLand& ptr) {
    ChooseOnlinePlayerUtilGUI::sendTo(player, [ptr](Player& self, Player& target) {
        _confirmTransferLand(self, ptr, target.getUuid(), target.getRealName());
    });
}
void LandManagerGUI::_sendTransferLandToOfflinePlayer(Player& player, SharedLand const& ptr) {
    CustomForm fm("[PLand] | 转让给离线玩家"_trf(player));
    fm.appendInput("playerName", "请输入离线玩家名称"_trf(player), "玩家名称");
    fm.sendTo(player, [ptr](Player& self, CustomFormResult const& res, FormCancelReason) {
        if (!res) {
            return;
        }
        auto playerName = std::get<std::string>(res->at("playerName"));
        if (playerName.empty()) {
            feedback_utils::sendErrorText(self, "玩家名称不能为空!"_trf(self));
            sendTransferLandGUI(self, ptr);
            return;
        }

        auto playerInfo = ll::service::PlayerInfo::getInstance().fromName(playerName);
        if (!playerInfo) {
            feedback_utils::sendErrorText(self, "未找到该玩家信息，请检查名称是否正确!"_trf(self));
            sendTransferLandGUI(self, ptr);
            return;
        }
        auto& targetUuid = playerInfo->uuid;
        _confirmTransferLand(self, ptr, targetUuid, playerName);
    });
}
void LandManagerGUI::_confirmTransferLand(
    Player&           player,
    const SharedLand& ptr,
    mce::UUID         target,
    std::string       displayName
) {
    ModalForm(
        "[PLand] | 确认转让"_trf(player),
        "您确定要将领地转让给 {} 吗?\n转让后，您将失去此领地的权限。\n此操作不可逆,请谨慎操作!"_trf(
            player,
            displayName
        ),
        "确认"_trf(player),
        "返回"_trf(player)
    )
        .sendTo(player, [ptr, target, displayName](Player& player, ModalFormResult const& res, FormCancelReason) {
            if (!res) {
                return;
            }
            if (!(bool)res.value()) {
                sendTransferLandGUI(player, ptr);
                return;
            }
            auto& service = PLand::getInstance().getServiceLocator().getLandManagementService();
            if (auto expected = service.transferLand(player, ptr, target)) {
                feedback_utils::sendText(player, "领地已转让给 {}"_trf(player, displayName));
                if (auto targetPlayer = player.getLevel().getPlayer(target)) {
                    feedback_utils::sendText(
                        *targetPlayer,
                        "您已接手来自 \"{}\" 的领地 \"{}\""_trf(player, player.getRealName(), ptr->getName())
                    );
                }
            } else {
                feedback_utils::sendError(player, expected.error());
            }
        });
}

void LandManagerGUI::sendCreateSubLandConfirm(Player& player, const SharedLand& ptr) {
    ModalForm{
        "[PLand] | 子领地创建确认"_trf(player),
        "将在当前领地内划出一个新的子领地\n\n子领地将成为独立领地：\n• 位于当前领地范围内\n• 权限完全独立（类似租界）\n是否继续？"_trf(
            player
        ),
        "创建子领地"_trf(player),
        "返回"_trf(player)
    }
        .sendTo(player, [ptr](Player& player, ModalFormResult const& res, FormCancelReason) {
            if (!res) return;
            if (!(bool)res.value()) {
                LandManagerGUI::sendMainMenu(player, ptr);
                return;
            }
            auto& service = PLand::getInstance().getServiceLocator().getLandManagementService();
            if (auto expected = service.requestCreateSubLand(player)) {
                feedback_utils::sendText(
                    player,
                    "选区功能已开启，使用命令 /pland set 或使用 {} 来选择ab点"_trf(player, Config::cfg.selector.tool)
                );
            } else {
                feedback_utils::sendError(player, expected.error());
            }
        });
}

void LandManagerGUI::sendChangeRangeConfirm(Player& player, SharedLand const& ptr) {
    ModalForm fm(
        "[PLand] | 重新选区"_trf(player),
        "重新选区为完全重新选择领地的范围，非直接扩充/缩小现有领地范围。\n重新选择的价格计算方式为\"新范围价格 — 旧范围价值\"，是否继续？"_trf(
            player
        ),
        "确认"_trf(player),
        "返回"_trf(player)
    );
    fm.sendTo(player, [ptr](Player& self, ModalFormResult const& res, FormCancelReason) {
        if (!res) {
            return;
        }

        if (!(bool)res.value()) {
            LandManagerGUI::sendMainMenu(self, ptr);
            return;
        }
        auto& service = PLand::getInstance().getServiceLocator().getLandManagementService();
        if (auto expected = service.requestChangeRange(self, ptr)) {
            feedback_utils::sendText(
                self,
                "选区功能已开启，使用命令 /pland set 或使用 {} 来选择ab点"_trf(self, Config::cfg.selector.tool)
            );
        } else {
            feedback_utils::sendError(self, expected.error());
        }
    });
}


void LandManagerGUI::sendChangeMember(Player& player, SharedLand ptr) {
    auto fm = BackSimpleForm<>::make<LandManagerGUI::sendMainMenu>(ptr);

    fm.appendButton("添加在线成员"_trf(player), "textures/ui/color_plus", "path", [ptr](Player& self) {
        _sendAddOnlineMember(self, ptr);
    });
    fm.appendButton("添加离线成员"_trf(player), "textures/ui/color_plus", "path", [ptr](Player& self) {
        _sendAddOfflineMember(self, ptr);
    });

    auto& infos = ll::service::PlayerInfo::getInstance();
    for (auto& member : ptr->getMembers()) {
        auto i = infos.fromUuid(member);
        fm.appendButton(i.has_value() ? i->name : member.asString(), [member, ptr](Player& self) {
            _confirmRemoveMember(self, ptr, member);
        });
    }

    fm.sendTo(player);
}
void LandManagerGUI::_sendAddOnlineMember(Player& player, SharedLand ptr) {
    ChooseOnlinePlayerUtilGUI::sendTo(
        player,
        [ptr](Player& self, Player& target) { _confirmAddMember(self, ptr, target.getUuid(), target.getRealName()); },
        BackSimpleForm<>::makeCallback<sendChangeMember>(ptr)
    );
}
void LandManagerGUI::_sendAddOfflineMember(Player& player, SharedLand ptr) {
    CustomForm fm("[PLand] | 添加离线成员"_trf(player));
    fm.appendInput("playerName", "请输入离线玩家名称"_trf(player), "玩家名称");
    fm.sendTo(player, [ptr](Player& self, CustomFormResult const& res, FormCancelReason) {
        if (!res) {
            return;
        }
        auto playerName = std::get<std::string>(res->at("playerName"));
        if (playerName.empty()) {
            feedback_utils::sendErrorText(self, "玩家名称不能为空!"_trf(self));
            sendChangeMember(self, ptr);
            return;
        }

        auto playerInfo = ll::service::PlayerInfo::getInstance().fromName(playerName);
        if (!playerInfo) {
            feedback_utils::sendErrorText(self, "未找到该玩家信息，请检查名称是否正确!"_trf(self));
            sendChangeMember(self, ptr);
            return;
        }

        auto& targetUuid = playerInfo->uuid;
        _confirmAddMember(self, ptr, targetUuid, playerName);
    });
}
void LandManagerGUI::_confirmAddMember(Player& player, SharedLand ptr, mce::UUID member, std::string displayName) {
    ModalForm fm(
        "[PLand] | 添加成员"_trf(player),
        "您确定要添加 {} 为领地成员吗?"_trf(player, displayName),
        "确认"_trf(player),
        "返回"_trf(player)
    );
    fm.sendTo(player, [ptr, member](Player& self, ModalFormResult const& res, FormCancelReason) {
        if (!res) {
            return;
        }
        if (!(bool)res.value()) {
            sendChangeMember(self, ptr);
            return;
        }

        auto& service = PLand::getInstance().getServiceLocator().getLandManagementService();
        if (auto expected = service.addMember(self, ptr, member)) {
            feedback_utils::sendText(self, "添加成功!"_trf(self));
        } else {
            feedback_utils::sendError(self, expected.error());
        }
    });
}
void LandManagerGUI::_confirmRemoveMember(Player& player, SharedLand ptr, mce::UUID member) {
    auto info = ll::service::PlayerInfo::getInstance().fromUuid(member);

    ModalForm fm(
        "[PLand] | 移除成员"_trf(player),
        "您确定要移除成员 \"{}\" 吗?"_trf(player, info.has_value() ? info->name : member.asString()),
        "确认"_trf(player),
        "返回"_trf(player)
    );
    fm.sendTo(player, [ptr, member](Player& self, ModalFormResult const& res, FormCancelReason) {
        if (!res) {
            return;
        }
        if (!(bool)res.value()) {
            sendChangeMember(self, ptr);
            return;
        }

        auto& service = PLand::getInstance().getServiceLocator().getLandManagementService();
        if (auto expected = service.removeMember(self, ptr, member)) {
            feedback_utils::sendText(self, "移除成员成功!"_trf(self));
        } else {
            feedback_utils::sendError(self, expected.error());
        }
    });
}


} // namespace land
