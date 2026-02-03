#pragma once
#include "pland/Global.h"

#include <mc/deps/core/math/Vec3.h>


namespace land {
struct LandResizeSettlement;
class Land;
class DefaultSelector;
class SubLandSelector;
class SelectorManager;
class LandResizeSelector;
class LandRegistry;
} // namespace land
namespace land::service {
class LandHierarchyService;
class LandPriceService;
} // namespace land::service

namespace land {
namespace service {

enum class DeletePolicy : uint8_t {
    CurrentOnly,      // 只删当前（普通 or 子）
    Recursive,        // 递归删除子领地
    PromoteChildren,  // 删除当前，子领地提升
    TransferChildren, // 删除当前，子领地转移
};

class LandManagementService {
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    LandManagementService(
        LandRegistry&         registry,
        SelectorManager&      selectorManager,
        LandHierarchyService& hierarchyService,
        LandPriceService&     priceService
    );
    ~LandManagementService();

    LD_DISABLE_COPY_AND_MOVE(LandManagementService);

    /**
     * 请求创建普通领地
     * @param player 玩家
     * @param is3D 领地类别
     */
    ll::Expected<> requestCreateOrdinaryLand(Player& player, bool is3D) const;

    /**
     * 请求创建子领地
     * @param player 玩家
     */
    ll::Expected<> requestCreateSubLand(Player& player);

    ll::Expected<> requestCreateSubLand(Player& player, std::shared_ptr<Land> const& land) const;

    /**
     * 购买普通领地
     * @param player 玩家
     * @param selector 选区
     * @param money 需支付经济
     * @return 领地对象
     */
    ll::Expected<std::shared_ptr<Land>> buyLand(Player& player, DefaultSelector* selector, int64_t money);

    /**
     * 购买子领地
     * @param player 玩家
     * @param selector 选区
     * @param money 需要支付的经济
     * @return 子领地对象
     */
    ll::Expected<std::shared_ptr<Land>> buyLand(Player& player, SubLandSelector* selector, int64_t money);

    /**
     * 处理领地范围变更
     * @param player 玩家
     * @param selector 选区
     * @param settlement 结算数据
     */
    ll::Expected<>
    handleChangeRange(Player& player, LandResizeSelector* selector, LandResizeSettlement const& settlement);

    ll::Expected<> ensurePlayerLandCountLimit(mce::UUID const& uuid) const;

    ll::Expected<> setLandTeleportPos(Player& player, std::shared_ptr<Land> const& land, Vec3 point);

    ll::Expected<> deleteLand(Player& player, std::shared_ptr<Land> ptr, DeletePolicy policy);

private:
    ll::Expected<std::shared_ptr<Land>>
    _payMoneyAndCreateOrdinaryLand(Player& player, DefaultSelector* selector, int64_t money);

    ll::Expected<> _addOrdinaryLand(Player& player, std::shared_ptr<Land> ptr);

    ll::Expected<std::shared_ptr<Land>>
    _payMoneyAndCreateSubLand(Player& player, SubLandSelector* selector, int64_t money);

    ll::Expected<> _ensureAndAttachSubLand(Player& player, std::shared_ptr<Land> parent, std::shared_ptr<Land> sub);

    ll::Expected<> _processResizeSettlement(Player& player, LandResizeSettlement const& settlement);

    ll::Expected<> _ensureLandWithDeletePolicy(Player& player, std::shared_ptr<Land> const& land, DeletePolicy policy);

    ll::Expected<> _processDeleteLand(Player& player, std::shared_ptr<Land> const& land, DeletePolicy policy);

    ll::Expected<> _processLandRefund(Player& player, std::shared_ptr<Land> const& land, bool isSingle);
};

} // namespace service
} // namespace land
