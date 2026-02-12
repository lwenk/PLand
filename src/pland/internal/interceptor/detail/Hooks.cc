#include "pland/internal/interceptor/EventInterceptor.h"
#include "pland/internal/interceptor/InterceptorConfig.h"
#include "pland/internal/interceptor/helper/InterceptorHelper.h"

#include "pland/PLand.h"
#include "pland/land/repo/LandRegistry.h"

#include "ll/api/event/EventBus.h"
#include "ll/api/event/entity/ActorHurtEvent.h"
#include "ll/api/memory/Hook.h"

#include "mc/server/ServerPlayer.h"
#include "mc/world/actor/ActorDamageSource.h"
#include "mc/world/actor/ActorType.h"
#include "mc/world/actor/FishingHook.h"
#include "mc/world/actor/Mob.h"
#include "mc/world/actor/ai/goal/LayEggGoal.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/block/FireBlock.h"
#include "mc/world/level/block/actor/ChestBlockActor.h"

#include "mc/world/actor/global/LightningBolt.h"

namespace land::internal::interceptor {

// Fix [#140](https://github.com/engsr6982/PLand/issues/140)
LL_TYPE_INSTANCE_HOOK(
    MobHurtHook,
    HookPriority::Normal,
    Mob,
    &Mob::$_hurt,
    bool,
    ::ActorDamageSource const& source,
    float                      damage,
    bool                       knock,
    bool                       ignite
) {
    ll::event::ActorHurtEvent ev{*this, source, damage, knock, ignite};
    ll::event::EventBus::getInstance().publish(ev);
    if (ev.isCancelled()) {
        return false;
    }
    return origin(source, damage, knock, ignite);
}

// Fix [#56](https://github.com/engsr6982/PLand/issues/56)
LL_TYPE_INSTANCE_HOOK(
    FishingHookHitHook,
    ll::memory::HookPriority::Normal,
    ::FishingHook,
    &::FishingHook::_pullCloser,
    void,
    Actor& inEntity,
    float  inSpeed
) {
    // 获取钓鱼钩的位置和维度
    auto& hookActor = *this;
    auto* player    = hookActor.getPlayerOwner();
    if (!player) {
        origin(inEntity, inSpeed);
        return;
    }

    auto& pos   = hookActor.getPosition();
    auto  dimId = hookActor.getDimensionId();

    auto& db   = PLand::getInstance().getLandRegistry();
    auto  land = db.getLandAt(pos, dimId);
    if (!hasRolePermission<&RolePerms::allowFishingRodAndHook>(land, player->getUuid())) {
        return;
    }

    origin(inEntity, inSpeed);
}

// Fix [#69](https://github.com/engsr6982/PLand/issues/69)
LL_TYPE_INSTANCE_HOOK(
    LayEggGoalHook,
    ll::memory::HookPriority::Normal,
    ::LayEggGoal,
    &::LayEggGoal::$isValidTarget,
    bool,
    ::BlockSource&    region,
    ::BlockPos const& pos
) {
    auto& db   = PLand::getInstance().getLandRegistry();
    auto  land = db.getLandAt(pos, region.getDimensionId());
    if (!hasEnvironmentPermission<&EnvironmentPerms::allowMobGrief>(land)) {
        return false; // 如果领地内不允许实体破坏，则阻止产蛋
    }
    return origin(region, pos);
}

// Fix [#136](https://github.com/engsr6982/PLand/issues/136)
LL_TYPE_INSTANCE_HOOK(
    FireBlockBurnHook,
    ll::memory::HookPriority::Normal,
    FireBlock,
    &FireBlock::checkBurn,
    void,
    ::BlockSource&    region,
    ::BlockPos const& pos,
    int               chance,
    ::Randomize&      randomize,
    int               age,
    ::BlockPos const& firePos
) {
    auto& db   = PLand::getInstance().getLandRegistry();
    auto  land = db.getLandAt(pos, region.getDimensionId());
    if (!hasEnvironmentPermission<&EnvironmentPerms::allowFireSpread>(land)) {
        return; // 如果领地内不允许火焰蔓延，则阻止蔓延
    }
    origin(region, pos, chance, randomize, age, firePos);
}


// Fix [#158](https://github.com/engsr6982/PLand/issues/158)
LL_TYPE_INSTANCE_HOOK(
    ChestBlockActorOpenHook,
    ll::memory::HookPriority::Normal,
    ChestBlockActor,
    &ChestBlockActor::$startOpen,
    void,
    ::Actor& actor
) {
    if (actor.isPlayer()) {
        origin(actor);
        return;
    }
    auto& db   = PLand::getInstance().getLandRegistry();
    auto  land = db.getLandAt(this->mPosition, actor.getDimensionId());
    if (!hasGuestPermission<&RolePerms::useContainer>(land)) {
        return; // 访客权限不允许，拦截铜傀儡开箱子
    }
    origin(actor);
}

// fix: [#167](https://github.com/IceBlcokMC/PLand/issues/167)
LL_TYPE_INSTANCE_HOOK(
    LightningBoltHook,
    ll::memory::HookPriority::Normal,
    LightningBolt,
    &LightningBolt::$normalTick,
    void
) {
    auto& registry = PLand::getInstance().getLandRegistry();
    if (auto land = registry.getLandAt(this->getPosition(), this->getDimensionId())) {
        if (!hasEnvironmentPermission<&EnvironmentPerms::allowLightningBolt>(land)) {
            this->remove(); // 必须标记移除，否则闪电实体不会被移除且会一直tick
            return;         // 不允许闪电，拦截 tick
        }
    }
    origin();
}

void EventInterceptor::setupHooks() {
    auto& config = InterceptorConfig::cfg.hooks;
    registerHookIf<MobHurtHook>(config.MobHurtHook);
    registerHookIf<FishingHookHitHook>(config.FishingHookHitHook);
    registerHookIf<LayEggGoalHook>(config.LayEggGoalHook);
    registerHookIf<FireBlockBurnHook>(config.FireBlockBurnHook);
    registerHookIf<ChestBlockActorOpenHook>(config.ChestBlockActorOpenHook);
    registerHookIf<LightningBoltHook>(config.LightningBoltHook);
}

} // namespace land::internal::interceptor