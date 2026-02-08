#pragma once
#include "pland/Global.h"
#include "pland/aabb/LandAABB.h"
#include <vector>


namespace land {

namespace legacy {
namespace v25 {
struct LandPermTable {
    // 标记 [x] 为复用权限
    bool allowFireSpread{true};           // 火焰蔓延
    bool allowAttackDragonEgg{false};     // 点击龙蛋
    bool allowFarmDecay{true};            // 耕地退化
    bool allowPistonPushOnBoundary{true}; // 活塞推动
    bool allowRedstoneUpdate{true};       // 红石更新
    bool allowExplode{false};             // 爆炸
    bool allowBlockFall{false};           // 方块掉落
    bool allowDestroy{false};             // 允许破坏
    bool allowWitherDestroy{false};       // 允许凋零破坏
    bool allowPlace{false};               // 允许放置 [x]
    bool allowPlayerDamage{false};        // 允许玩家受伤
    bool allowMonsterDamage{true};        // 允许敌对生物受伤
    bool allowPassiveDamage{false};       // 允许友好、中立生物受伤
    bool allowSpecialDamage{false};       // 允许对特殊实体造成伤害(船、矿车、画等)
    bool allowCustomSpecialDamage{false}; // 允许对特殊实体2造成伤害
    bool allowOpenChest{false};           // 允许打开箱子
    bool allowPickupItem{false};          // 允许拾取物品
    bool allowEndermanLeaveBlock{false};  // 允许末影人放下方块

    bool allowDropItem{true};          // 允许丢弃物品
    bool allowProjectileCreate{false}; // 允许弹射物创建
    bool allowRideEntity{false};       // 允许骑乘实体
    bool allowRideTrans{false};        // 允许骑乘矿车、船
    bool allowAxePeeled{false};        // 允许斧头去皮
    bool allowLiquidFlow{true};        // 允许液体流动
    bool allowSculkBlockGrowth{true};  // 允许幽匿尖啸体生长
    bool allowMonsterSpawn{true};      // 允许怪物生成
    bool allowAnimalSpawn{true};       // 允许动物生成
    bool allowInteractEntity{false};   // 实体交互
    bool allowActorDestroy{false};     // 实体破坏


    bool useAnvil{false};               // 使用铁砧
    bool useBarrel{false};              // 使用木桶
    bool useBeacon{false};              // 使用信标
    bool useBed{false};                 // 使用床
    bool useBell{false};                // 使用钟
    bool useBlastFurnace{false};        // 使用高炉
    bool useBrewingStand{false};        // 使用酿造台
    bool useCampfire{false};            // 使用营火
    bool useFlintAndSteel{false};       // 使用打火石
    bool useCartographyTable{false};    // 使用制图台
    bool useComposter{false};           // 使用堆肥桶
    bool useCraftingTable{false};       // 使用工作台
    bool useDaylightDetector{false};    // 使用阳光探测器
    bool useDispenser{false};           // 使用发射器
    bool useDropper{false};             // 使用投掷器
    bool useEnchantingTable{false};     // 使用附魔台
    bool useDoor{false};                // 使用门
    bool useFenceGate{false};           // 使用栅栏门
    bool useFurnace{false};             // 使用熔炉
    bool useGrindstone{false};          // 使用砂轮
    bool useHopper{false};              // 使用漏斗
    bool useJukebox{false};             // 使用唱片机
    bool useLoom{false};                // 使用织布机
    bool useStonecutter{false};         // 使用切石机
    bool useNoteBlock{false};           // 使用音符盒
    bool useCrafter{false};             // 使用合成器
    bool useChiseledBookshelf{false};   // 使用雕纹书架
    bool useCake{false};                // 吃蛋糕
    bool useComparator{false};          // 使用红石比较器
    bool useRepeater{false};            // 使用红石中继器
    bool useShulkerBox{false};          // 使用潜影盒
    bool useSmithingTable{false};       // 使用锻造台
    bool useSmoker{false};              // 使用烟熏炉
    bool useTrapdoor{false};            // 使用活板门
    bool useLectern{false};             // 使用讲台
    bool useCauldron{false};            // 使用炼药锅
    bool useLever{false};               // 使用拉杆
    bool useButton{false};              // 使用按钮
    bool useRespawnAnchor{false};       // 使用重生锚
    bool useItemFrame{false};           // 使用物品展示框
    bool allowFishingRodAndHook{false}; // 使用钓鱼竿
    bool useBucket{false};              // 使用桶
    bool usePressurePlate{false};       // 使用压力板
    bool useArmorStand{false};          // 使用盔甲架
    bool useBoneMeal{false};            // 使用骨粉
    bool useHoe{false};                 // 使用锄头
    bool useShovel{false};              // 使用锹
    bool useVault{false};               // 使用试炼宝库
    bool useBeeNest{false};             // 使用蜂巢蜂箱
    bool placeBoat{false};              // 放置船
    bool placeMinecart{false};          // 放置矿车

    bool editFlowerPot{false}; // 编辑花盆
    bool editSign{false};      // 编辑告示牌
};
} // namespace v25
} // namespace impl


struct EnvironmentPerms final {
    bool allowFireSpread;           // 火焰蔓延
    bool allowMonsterSpawn;         // 怪物生成
    bool allowAnimalSpawn;          // 动物生成
    bool allowMobGrief;             // 实体破坏(破坏方块/拾取方块/放置方块)
    bool allowExplode;              // 爆炸
    bool allowFarmDecay;            // 耕地退化
    bool allowPistonPushOnBoundary; // 活塞推动边界方块
    bool allowRedstoneUpdate;       // 红石更新
    bool allowBlockFall;            // 方块掉落
    bool allowWitherDestroy;        // 凋零破坏
    bool allowMossGrowth;           // 苔藓生长(蔓延)
    bool allowLiquidFlow;           // 流动液体
    bool allowDragonEggTeleport;    // 龙蛋传送
    bool allowSculkBlockGrowth;     // 幽匿尖啸体生长
    bool allowSculkSpread;          // 幽匿蔓延
};
struct RolePerms final {
    struct Entry final {
        bool member;
        bool guest;
    };
    Entry allowDestroy{};           // 允许破坏方块
    Entry allowPlace{};             // 允许放置方块
    Entry useBucket{};              // 允许使用桶(水/岩浆/...)
    Entry useAxe{};                 // 允许使用斧头
    Entry useHoe{};                 // 允许使用锄头
    Entry useShovel{};              // 允许使用铲子
    Entry placeBoat{};              // 允许放置船
    Entry placeMinecart{};          // 允许放置矿车
    Entry useButton{};              // 允许使用按钮
    Entry useDoor{};                // 允许使用门
    Entry useFenceGate{};           // 允许使用栅栏门
    Entry allowInteractEntity{};    // 允许与实体交互 // TODO: 解决歧义：玩家交互实体 & 玩家取走栅栏上的拴绳实体
    Entry useTrapdoor{};            // 允许使用活板门
    Entry editSign{};               // 允许编辑告示牌
    Entry useLever{};               // 允许使用拉杆
    Entry useFurnaces{};            // 允许使用所有熔炉类方块（熔炉/高炉/烟熏炉）
    Entry allowPlayerPickupItem{};  // 允许玩家拾取物品
    Entry allowRideTrans{};         // 允许骑乘运输工具（矿车/船）
    Entry allowRideEntity{};        // 允许骑乘实体
    Entry usePressurePlate{};       // 触发压力板
    Entry allowFishingRodAndHook{}; // 允许使用钓鱼竿和鱼钩
    Entry allowProjectileCreate{};  // 允许弹射物创建
    Entry useArmorStand{};          // 允许使用盔甲架
    Entry allowDropItem{};          // 允许丢弃物品
    Entry useItemFrame{};           // 允许操作物品展示框
    Entry useFlintAndSteel{};       // 使用打火石
    Entry useBeacon{};              // 使用信标
    Entry useBed{};                 // 使用床

    // 以下权限均通过 PermMapping 动态映射
    Entry allowPvP{};                 // 允许PvP
    Entry allowHostileDamage{};       // 敌对生物受到伤害
    Entry allowFriendlyDamage{};      // 友好生物受到伤害
    Entry allowSpecialEntityDamage{}; // 特殊生物受到伤害
    Entry useContainer{};             // 允许使用容器(箱子/木桶/潜影盒/发射器/投掷器/漏斗/雕纹书架/试炼宝库/...)
    Entry useWorkstation{};           // 工作站类(工作台/铁砧/附魔台/酿造台/锻造台/砂轮/织布机/切石机/制图台/合成器)
    Entry useBell{};                  // 使用钟
    Entry useCampfire{};              // 使用营火
    Entry useComposter{};             // 使用堆肥桶
    Entry useDaylightDetector{};      // 使用阳光探测器
    Entry useJukebox{};               // 使用唱片机
    Entry useNoteBlock{};             // 使用音符盒
    Entry useCake{};                  // 吃蛋糕
    Entry useComparator{};            // 使用红石比较器
    Entry useRepeater{};              // 使用红石中继器
    Entry useLectern{};               // 使用讲台
    Entry useCauldron{};              // 使用炼药锅
    Entry useRespawnAnchor{};         // 使用重生锚
    Entry useBoneMeal{};              // 使用骨粉
    Entry useBeeNest{};               // 使用蜂巢(蜂箱)
    Entry editFlowerPot{};            // 编辑花盆
};
struct LandPermTable final {
    EnvironmentPerms environment{};
    RolePerms        role{};
};

// ! 注意：如果 LandContext 有更改，则必须递增 LandSchemaVersion，否则导致加载异常
// 对于字段变动、重命名，请注册对应的 migrator 转换数据
constexpr int LandSchemaVersion = 27;
struct LandContext {
    int                      version{LandSchemaVersion};            // 版本号
    LandAABB                 mPos{};                                // 领地对角坐标
    LandPos                  mTeleportPos{};                        // 领地传送坐标
    LandID                   mLandID{INVALID_LAND_ID};              // 领地唯一ID  (由 LandRegistry::addLand() 时分配)
    LandDimid                mLandDimid{};                          // 领地所在维度
    bool                     mIs3DLand{};                           // 是否为3D领地
    LandPermTable            mLandPermTable{};                      // 领地权限
    std::string              mLandOwner{};                          // 领地主人(默认UUID,其余情况看mOwnerDataIsXUID)
    std::vector<std::string> mLandMembers{};                        // 领地成员
    std::string              mLandName{"Unnamed territories"_tr()}; // 领地名称
    int                      mOriginalBuyPrice{0};                  // 原始购买价格
    [[deprecated]] bool      mIsConvertedLand{false};               // 是否为转换后的领地(其它插件创建的领地)
    [[deprecated]] bool      mOwnerDataIsXUID{false}; // 领地主人数据是否为XUID (如果为true，则主人上线自动转换为UUID)
    LandID                   mParentLandID{INVALID_LAND_ID}; // 父领地ID
    std::vector<LandID>      mSubLandIDs{};                  // 子领地ID
};

STATIC_ASSERT_AGGREGATE(LandPermTable);
STATIC_ASSERT_AGGREGATE(LandContext);
template <typename T, typename I>
concept AssertPosField = requires(T const& t, I const& i) {
    { t.min } -> std::convertible_to<I>;
    { t.max } -> std::convertible_to<I>;
    { i.x } -> std::convertible_to<int>;
    { i.y } -> std::convertible_to<int>;
    { i.z } -> std::convertible_to<int>;
};
static_assert(
    AssertPosField<LandAABB, LandPos>,
    "If the field is changed, please update the data transformation rules accordingly."
);

} // namespace land