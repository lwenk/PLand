#pragma once
#include "pland/Global.h"
#include "pland/land/Land.h"

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class Player;
class BlockPos;

namespace land {

struct PlayerSettings {
    bool showEnterLandTitle{true};     // 是否显示进入领地提示
    bool showBottomContinuedTip{true}; // 是否持续显示底部提示
};

class LandTemplatePermTable;

class LandRegistry final {
    struct Impl;
    std::unique_ptr<Impl> impl;

    friend class TransactionContext;

    LandID _allocateNextId();

public:
    LD_DISABLE_COPY_AND_MOVE(LandRegistry);
    explicit LandRegistry();
    ~LandRegistry();

    LDAPI void save();

    LDAPI bool save(std::shared_ptr<Land> const& land, bool force = false) const;

public:
    LDNDAPI bool isOperator(mce::UUID const& uuid) const;

    LDNDAPI bool addOperator(mce::UUID const& uuid);

    LDNDAPI bool removeOperator(mce::UUID const& uuid);

    LDNDAPI std::vector<mce::UUID> const& getOperators() const;

    LDNDAPI PlayerSettings& getOrCreatePlayerSettings(mce::UUID const& uuid);

    LDNDAPI LandTemplatePermTable& getLandTemplatePermTable() const;

    LDNDAPI bool hasLand(LandID id) const;

    LDAPI void refreshLandRange(SharedLand const& ptr); // 刷新领地范围

    LDNDAPI ll::Expected<> addOrdinaryLand(SharedLand const& land);

    LDNDAPI ll::Expected<> removeOrdinaryLand(SharedLand const& ptr);


    /**
     * 子领地事务执行器
     * @note 在执行前，Registry 会对当前领地数据进行快照，如果任务返回 false，则回滚数据到快照
     * @note 在回调内，禁止再次访问 Registry，这会造成死锁
     */
    using TransactionCallback = std::function<bool(TransactionContext& ctx)>;

    LDNDAPI ll::Expected<> executeTransaction(
        std::unordered_set<std::shared_ptr<Land>> const& participants,
        TransactionCallback const&                       executor
    );


public: // 领地查询API
    LDNDAPI SharedLand getLand(LandID id) const;
    LDNDAPI std::vector<SharedLand> getLands() const;
    LDNDAPI std::vector<SharedLand> getLands(std::vector<LandID> const& ids) const;
    LDNDAPI std::vector<SharedLand> getLands(LandDimid dimid) const;
    LDNDAPI std::vector<SharedLand> getLands(mce::UUID const& uuid, bool includeShared = false) const;
    LDNDAPI std::vector<SharedLand> getLands(mce::UUID const& uuid, LandDimid dimid) const;
    LDNDAPI std::unordered_map<mce::UUID, std::unordered_set<SharedLand>> getLandsByOwner() const;
    LDNDAPI std::unordered_map<mce::UUID, std::unordered_set<SharedLand>> getLandsByOwner(LandDimid dimid) const;

    LDNDAPI LandPermType getPermType(mce::UUID const& uuid, LandID id = 0, bool includeOperator = true) const;

    LDNDAPI SharedLand getLandAt(BlockPos const& pos, LandDimid dimid) const;

    LDNDAPI std::unordered_set<SharedLand> getLandAt(BlockPos const& center, int radius, LandDimid dimid) const;

    LDNDAPI std::unordered_set<SharedLand> getLandAt(BlockPos const& pos1, BlockPos const& pos2, LandDimid dimid) const;

    using ContextFilter = std::function<bool(LandContext const&)>;
    LDNDAPI std::vector<SharedLand> getLandsWhereRaw(ContextFilter const& filter) const;

public:
    LDAPI static ChunkID             EncodeChunkID(int x, int z);
    LDAPI static std::pair<int, int> DecodeChunkID(ChunkID id);

    static constexpr auto DbDirName              = "db";              // 数据库目录名
    static constexpr auto DbVersionKey           = "__version__";     // 数据库版本键
    static constexpr auto DbOperatorDataKey      = "operators";       // 操作员数据键
    static constexpr auto DbPlayerSettingDataKey = "player_settings"; // 玩家设置数据键
    static constexpr auto DbTemplatePermKey      = "template_perm";   // 领地模板权限表数据键
    static bool           isLandData(std::string_view key);           // 判断键是否为领地数据键
};


} // namespace land
