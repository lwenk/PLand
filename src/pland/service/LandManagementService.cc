#include "LandManagementService.h"

#include "LandHierarchyService.h"
#include "pland/events/domain/LandResizedEvent.h"
#include "pland/events/player/PlayerBuyLandEvent.h"
#include "pland/events/player/PlayerChangeLandRangeEvent.h"
#include "pland/events/player/PlayerRequestCreateLandEvent.h"
#include "pland/infra/Config.h"
#include "pland/land/LandCreateValidator.h"
#include "pland/land/LandRegistry.h"
#include "pland/land/LandResizeSettlement.h"
#include "pland/selector/DefaultSelector.h"
#include "pland/selector/LandResizeSelector.h"
#include "pland/selector/SelectorManager.h"
#include "pland/selector/SubLandSelector.h"

#include <ll/api/event/EventBus.h>

#include <mc/world/actor/player/Player.h>

namespace land {
namespace service {

struct LandManagementService::Impl {
    LandRegistry&         mRegistry;
    SelectorManager&      mSelectorManager;
    LandHierarchyService& mLandHierarchyService;
};

LandManagementService::LandManagementService(
    LandRegistry&         registry,
    SelectorManager&      selectorManager,
    LandHierarchyService& service
)
: impl(std::make_unique<Impl>(registry, selectorManager, service)) {}
LandManagementService::~LandManagementService() {}


ll::Expected<> LandManagementService::requestCreateOrdinaryLand(Player& player, bool is3D) const {
    if (!Config::ensureDimensionAllowed(player.getDimensionId())) {
        return ll::makeStringError("你所在的维度无法购买领地"_trf(player));
    }
    if (!Config::ensureOrdinaryLandEnabled(is3D)) {
        return ll::makeStringError("当前领地类型未启用，请联系管理员"_trf(player));
    }

    ll::event::EventBus::getInstance().publish(event::PlayerRequestCreateLandEvent{player, LandType::Ordinary});

    auto selector = std::make_unique<DefaultSelector>(player, is3D);
    if (!impl->mSelectorManager.startSelection(std::move(selector))) {
        return ll::makeStringError("选区开启失败，当前存在未完成的选区任务"_trf(player));
    }

    return {};
}

ll::Expected<> LandManagementService::requestCreateSubLand(Player& player) {
    if (!Config::ensureSubLandFeatureEnabled()) {
        return ll::makeStringError("子领地功能未启用"_trf(player));
    }
    if (!Config::ensureDimensionAllowed(player.getDimensionId())) {
        return ll::makeStringError("你所在的维度无法购买领地"_trf(player));
    }

    auto land = impl->mRegistry.getLandAt(player.getPosition(), player.getDimensionId());
    if (!land || !land->isOwner(player.getUuid())) {
        return ll::makeStringError("操作失败, 当前位置没有领地或您不是当前领地主人"_trf(player));
    }
    if (!land->canCreateSubLand()) {
        return ll::makeStringError("操作失败，当前领地无法继续创建子领地"_trf(player));
    }

    ll::event::EventBus::getInstance().publish(event::PlayerRequestCreateLandEvent{player, LandType::Sub});

    auto selector = std::make_unique<SubLandSelector>(player, land);
    if (!impl->mSelectorManager.startSelection(std::move(selector))) {
        return ll::makeStringError("选区开启失败，当前存在未完成的选区任务"_trf(player));
    }

    return {};
}

ll::Expected<std::shared_ptr<Land>>
LandManagementService::buyLand(Player& player, DefaultSelector* selector, int64_t money) {
    assert(selector);
    auto event = event::PlayerBuyLandBeforeEvent{player, money, LandType::Ordinary};
    ll::event::EventBus::getInstance().publish(event);
    if (event.isCancelled()) {
        return ll::makeStringError("操作失败，请求被取消"_trf(player));
    }

    auto exp = _payMoneyAndCreateOrdinaryLand(player, selector, money);
    if (!exp) {
        return exp;
    }
    impl->mSelectorManager.stopSelection(player);
    ll::event::EventBus::getInstance().publish(event::PlayerBuyLandAfterEvent{player, exp.value(), money});
    return exp.value();
}

ll::Expected<std::shared_ptr<Land>>
LandManagementService::buyLand(Player& player, SubLandSelector* selector, int64_t money) {
    assert(selector != nullptr);
    auto event = event::PlayerBuyLandBeforeEvent{player, money, LandType::Sub};
    ll::event::EventBus::getInstance().publish(event);
    if (event.isCancelled()) {
        return ll::makeStringError("操作失败，请求被取消"_trf(player));
    }
    auto exp = _payMoneyAndCreateSubLand(player, selector, money);
    if (!exp) {
        return exp;
    }
    impl->mSelectorManager.stopSelection(player);
    ll::event::EventBus::getInstance().publish(event::PlayerBuyLandAfterEvent{player, exp.value(), money});
    return exp.value();
}
ll::Expected<> LandManagementService::handleChangeRange(
    Player&                     player,
    LandResizeSelector*         selector,
    LandResizeSettlement const& settlement
) {
    auto land = selector->getLand();
    if (!land) {
        return ll::makeStringError("获取领地失败"_trf(player));
    }
    if (!land->isOrdinaryLand()) {
        return ll::makeStringError("操作失败，当前领地无法调整大小"_trf(player));
    }

    auto range = selector->newLandAABB();
    assert(range.has_value());

    auto event = event::PlayerChangeLandRangeEvent{player, land, *range, settlement};
    ll::event::EventBus::getInstance().publish(event);
    if (event.isCancelled()) {
        return ll::makeStringError("操作失败，请求被取消"_trf(player));
    }

    if (auto res = LandCreateValidator::validateChangeLandRange(impl->mRegistry, land, *range); !res) {
        if (res.error().isA<LandCreateValidator::ValidateError>()) {
            auto& error = res.error().as<LandCreateValidator::ValidateError>();
            return ll::makeStringError(error.translateError(player.getLocaleCode()));
        }
        return ll::makeStringError(res.error().message());
    }
    if (auto res = _processResizeSettlement(player, settlement); !res) {
        return res;
    }
    land->_setAABB(*range);
    land->setOriginalBuyPrice(settlement.newTotalPrice);
    impl->mRegistry.refreshLandRange(land);
    impl->mSelectorManager.stopSelection(player);
    ll::event::EventBus::getInstance().publish(event::LandResizedEvent{land, *range});
    return {};
}

ll::Expected<> LandManagementService::requestDeleteLand(Player& player, std::shared_ptr<Land> land) {
    return {}; // TODO: impl
}
ll::Expected<> LandManagementService::ensurePlayerLandCountLimit(mce::UUID const& uuid) const {
    return LandCreateValidator::isPlayerLandCountLimitExceeded(impl->mRegistry, uuid);
}
ll::Expected<>
LandManagementService::setLandTeleportPos(Player& player, std::shared_ptr<Land> const& land, Vec3 point) {
    if (!land->isOwner(player.getUuid()) && !impl->mRegistry.isOperator(player.getUuid())) {
        return ll::makeStringError("操作失败，您不是领地主人"_trf(player));
    }
    if (!land->getAABB().hasPos(point)) {
        return ll::makeStringError("设置传送点失败，传送点不在领地范围内"_trf(player));
    }
    if (!land->setTeleportPos(LandPos::make(point))) {
        return ll::makeStringError("设置传送点失败"_trf(player));
    }
    return {};
}

ll::Expected<std::shared_ptr<Land>>
LandManagementService::_payMoneyAndCreateOrdinaryLand(Player& player, DefaultSelector* selector, int64_t money) {
    assert(selector != nullptr);
    auto& economy = EconomySystem::getInstance();
    if (!economy->reduce(player, money)) {
        return ll::makeStringError("您的余额不足，无法购买"_trf(player));
    }
    auto land = selector->newLand();
    land->setOriginalBuyPrice(money);

    auto exp = _addOrdinaryLand(player, land);
    if (!exp) {
        (void)economy->add(player, money);
        return ll::makeStringError(exp.error().message());
    }
    return land;
}

ll::Expected<> LandManagementService::_addOrdinaryLand(Player& player, std::shared_ptr<Land> ptr) {
    assert(ptr != nullptr);
    if (auto res = LandCreateValidator::validateCreateOrdinaryLand(impl->mRegistry, player, ptr); !res) {
        if (res.error().isA<LandCreateValidator::ValidateError>()) {
            auto& error = res.error().as<LandCreateValidator::ValidateError>();
            return ll::makeStringError(error.translateError(player.getLocaleCode()));
        }
        return ll::makeStringError(res.error().message());
    }
    return impl->mRegistry.addOrdinaryLand(ptr);
}

ll::Expected<std::shared_ptr<Land>>
LandManagementService::_payMoneyAndCreateSubLand(Player& player, SubLandSelector* selector, int64_t money) {
    assert(selector != nullptr);
    auto& economy = EconomySystem::getInstance();
    if (!economy->reduce(player, money)) {
        return ll::makeStringError("您的余额不足，无法购买"_trf(player));
    }
    auto parent = selector->getParentLand();
    assert(parent != nullptr);

    auto sub = selector->newSubLand();
    sub->setOriginalBuyPrice(money);

    auto exp = _ensureAndAttachSubLand(player, parent, sub);
    if (!exp) {
        (void)economy->add(player, money);
        return ll::makeStringError(exp.error().message());
    }
    return sub;
}

ll::Expected<> LandManagementService::_ensureAndAttachSubLand(
    Player&               player,
    std::shared_ptr<Land> parent,
    std::shared_ptr<Land> sub
) {
    assert(parent != nullptr);
    assert(sub != nullptr);

    auto res = LandCreateValidator::validateCreateSubLand(
        player,
        parent,
        sub->getAABB(),
        impl->mRegistry,
        impl->mLandHierarchyService
    );
    if (!res) {
        if (res.error().isA<LandCreateValidator::ValidateError>()) {
            auto& error = res.error().as<LandCreateValidator::ValidateError>();
            return ll::makeStringError(error.translateError(player.getLocaleCode()));
        }
        return ll::makeStringError(res.error().message());
    }
    return impl->mLandHierarchyService.attachSubLand(parent, sub);
}

ll::Expected<> LandManagementService::_processResizeSettlement(Player& player, LandResizeSettlement const& settlement) {
    auto& economy = EconomySystem::getInstance();
    switch (settlement.type) {
    case LandResizeSettlement::Type::Pay:
        if (!economy->reduce(player, settlement.amount)) {
            return ll::makeStringError("您的余额不足，无法购买"_trf(player));
        }
        break;
    case LandResizeSettlement::Type::Refund:
        if (!economy->add(player, settlement.amount)) {
            return ll::makeStringError("经济系统异常,退还差价失败"_trf(player));
        }
        break;
    default:;
    }
    return {};
}


} // namespace service
} // namespace land