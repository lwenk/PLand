#include "LandManagementService.h"

#include "pland/events/PlayerRequestCreateLandEvent.h"
#include "pland/infra/Config.h"
#include "pland/land/LandRegistry.h"
#include "pland/selector/DefaultSelector.h"
#include "pland/selector/SelectorManager.h"
#include "pland/selector/SubLandSelector.h"

#include <ll/api/event/EventBus.h>

#include <mc/world/actor/player/Player.h>

namespace land {
namespace service {

struct LandManagementService::Impl {
    LandRegistry&    mRegistry;
    SelectorManager& mSelectorManager;
};

LandManagementService::LandManagementService(LandRegistry& registry, SelectorManager& selectorManager)
: impl_(std::make_unique<Impl>(registry, selectorManager)) {}
LandManagementService::~LandManagementService() {}


ll::Expected<> LandManagementService::requestCreateOrdinaryLand(Player& player, bool is3D) const {
    if (!ensureDimensionAllowed(player)) {
        return ll::makeStringError("你所在的维度无法购买领地"_trf(player));
    }
    if (!ensureOrdinaryLandEnabled(is3D)) {
        return ll::makeStringError("当前领地类型未启用，请联系管理员"_trf(player));
    }

    ll::event::EventBus::getInstance().publish(
        event::PlayerRequestCreateLandEvent{player, event::PlayerRequestCreateLandEvent::Type::Ordinary}
    );

    auto selector = std::make_unique<DefaultSelector>(player, is3D);
    if (!impl_->mSelectorManager.startSelection(std::move(selector))) {
        return ll::makeStringError("选区开启失败，当前存在未完成的选区任务"_trf(player));
    }

    return {};
}

ll::Expected<> LandManagementService::requestCreateSubLand(Player& player) {
    if (!ensureSubLandFeatureEnabled()) {
        return ll::makeStringError("子领地功能未启用"_trf(player));
    }
    if (!ensureDimensionAllowed(player)) {
        return ll::makeStringError("你所在的维度无法购买领地"_trf(player));
    }

    auto land = impl_->mRegistry.getLandAt(player.getPosition(), player.getDimensionId());
    if (!land || !land->isOwner(player.getUuid())) {
        return ll::makeStringError("操作失败, 当前位置没有领地或您不是当前领地主人"_trf(player));
    }
    if (!land->canCreateSubLand()) {
        return ll::makeStringError("操作失败，当前领地无法继续创建子领地"_trf(player));
    }

    ll::event::EventBus::getInstance().publish(
        event::PlayerRequestCreateLandEvent{player, event::PlayerRequestCreateLandEvent::Type::Sub}
    );

    auto selector = std::make_unique<SubLandSelector>(player, land);
    if (!impl_->mSelectorManager.startSelection(std::move(selector))) {
        return ll::makeStringError("选区开启失败，当前存在未完成的选区任务"_trf(player));
    }

    return {};
}


bool LandManagementService::ensureDimensionAllowed(Player& player) {
    auto& allowed = Config::cfg.land.bought.allowDimensions;
    return std::find(allowed.begin(), allowed.end(), player.getDimensionId()) != allowed.end();
}

bool LandManagementService::ensureSubLandFeatureEnabled() { return Config::cfg.land.subLand.enabled; }

bool LandManagementService::ensureOrdinaryLandEnabled(bool is3D) {
    return is3D ? Config::cfg.land.bought.threeDimensionl.enabled : Config::cfg.land.bought.twoDimensionl.enabled;
}


} // namespace service
} // namespace land