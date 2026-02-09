#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

#include <mc/deps/core/string/HashedString.h>

namespace land::internal::interceptor {

struct InterceptorConfig {
    inline static int SchemaVersion = 1;

    int version = SchemaVersion;
    struct Listeners {
        bool PlayerDestroyBlockEvent              = true; // LL
        bool PlayerPlacingBlockEvent              = true; // LL
        bool PlayerInteractBlockEvent             = true; // LL
        bool PlayerAttackEvent                    = true; // LL
        bool PlayerPickUpItemEvent                = true; // LL
        bool SpawnedMobEvent                      = true; // LL (env)
        bool ActorHurtEvent                       = true; // LL
        bool FireSpreadEvent                      = true; // LL (env)
        bool ActorDestroyBlockEvent               = true; // ILA (env)
        bool MobTakeBlockBeforeEvent              = true; // ILA (env)
        bool MobPlaceBlockBeforeEvent             = true; // ILA (env)
        bool ActorRideBeforeEvent                 = true; // ILA
        bool MobHurtEffectBeforeEvent             = true; // ILA
        bool ActorTriggerPressurePlateBeforeEvent = true; // ILA
        bool ProjectileCreateBeforeEvent          = true; // ILA
        bool PlayerInteractEntityBeforeEvent      = true; // ILA
        bool ArmorStandSwapItemBeforeEvent        = true; // ILA
        bool PlayerDropItemBeforeEvent            = true; // ILA
        bool PlayerOperatedItemFrameBeforeEvent   = true; // ILA
        bool PlayerEditSignBeforeEvent            = true; // ILA
        bool ExplosionBeforeEvent                 = true; // ILA (env)
        bool FarmDecayBeforeEvent                 = true; // ILA (env)
        bool PistonPushBeforeEvent                = true; // ILA (env)
        bool RedstoneUpdateBeforeEvent            = true; // ILA (env)
        bool BlockFallBeforeEvent                 = true; // ILA (env)
        bool WitherDestroyBeforeEvent             = true; // ILA (env)
        bool MossGrowthBeforeEvent                = true; // ILA (env)
        bool LiquidFlowBeforeEvent                = true; // ILA (env)
        bool DragonEggBlockTeleportBeforeEvent    = true; // ILA (env)
        bool SculkBlockGrowthBeforeEvent          = true; // ILA (env)
        bool SculkSpreadBeforeEvent               = true; // ILA (env)
    } listeners;
    struct Hooks {
        bool MobHurtHook{true};             // 注册生物受伤Hook
        bool FishingHookHitHook{true};      // 注册钓鱼竿Hook
        bool LayEggGoalHook{true};          // 注册产卵AI目标Hook
        bool FireBlockBurnHook{true};       // 注册火焰蔓延Hook
        bool ChestBlockActorOpenHook{true}; // 注册箱子打开Hook
    } hooks;
    struct Rules {
        using Mapping = std::unordered_map<std::string, std::string>; // TypeName -> Permission
        struct Mob {
            using TypeNamesSet = std::unordered_set<HashedString>;
            TypeNamesSet allowHostileDamage;
            TypeNamesSet allowFriendlyDamage;
            TypeNamesSet allowSpecialEntityDamage;
        } mob;
        Mapping item;
        Mapping block;
    } rules;


    static InterceptorConfig cfg;

    inline static constexpr std::string_view FileName = "InterceptorConfig.json";

    static void load(std::filesystem::path configDir);
    static void save(std::filesystem::path configDir);

    static void tryMigrate(std::filesystem::path configDir);
};


} // namespace land::internal::interceptor
