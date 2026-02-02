#include "LandRegistry.h"
#include "StorageError.h"
#include "TransactionContext.h"
#include "pland/Global.h"
#include "pland/PLand.h"
#include "pland/aabb/LandAABB.h"
#include "pland/land/Land.h"
#include "pland/land/LandCreateValidator.h"
#include "pland/land/LandTemplatePermTable.h"
#include "pland/land/internal/LandDimensionChunkMap.h"
#include "pland/land/internal/LandIdAllocator.h"
#include "pland/land/repo/LandContext.h"
#include "pland/utils/JsonUtil.h"

#include "ll/api/Expected.h"
#include "ll/api/data/KeyValueDB.h"
#include "ll/api/i18n/I18n.h"

#include "mc/platform/UUID.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockPos.h"

#include "nlohmann/json_fwd.hpp"

#include "fmt/core.h"
#include "internal/LandMigrator.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>


namespace land {

struct LandRegistry::Impl {
    std::unique_ptr<ll::data::KeyValueDB>         mDB;                             // 领地数据库
    std::vector<mce::UUID>                        mLandOperators;                  // 领地操作员
    std::unordered_map<mce::UUID, PlayerSettings> mPlayerSettings;                 // 玩家设置
    std::unordered_map<LandID, SharedLand>        mLandCache;                      // 领地缓存
    mutable std::shared_mutex                     mMutex;                          // 读写锁
    std::unique_ptr<internal::LandIdAllocator>    mLandIdAllocator{nullptr};       // 领地ID分配器
    internal::LandDimensionChunkMap               mDimensionChunkMap;              // 维度区块映射
    std::unique_ptr<LandTemplatePermTable>        mLandTemplatePermTable{nullptr}; // 领地模板权限表
    std::thread                                   mThread;                         // 线程
    std::atomic<bool>                             mThreadQuit{false};              // 线程退出标志
    mutable std::mutex                            mThreadMutex;                    // 线程互斥锁(仅 mThreadCV 使用)
    std::condition_variable                       mThreadCV;                       // 线程条件变量


    void _loadOperators() {
        if (!mDB->has(DbOperatorDataKey)) {
            mDB->set(DbOperatorDataKey, "[]"); // empty array
        }
        auto ops = nlohmann::json::parse(*mDB->get(DbOperatorDataKey));
        for (auto& op : ops) {
            auto uuidStr = op.get<std::string>();
            if (!mce::UUID::canParse(uuidStr)) {
                PLand::getInstance().getSelf().getLogger().warn("Invalid operator UUID: {}", uuidStr);
            }
            mLandOperators.emplace_back(uuidStr);
        }
    }
    void _loadPlayerSettings() {
        if (!mDB->has(DbPlayerSettingDataKey)) {
            mDB->set(DbPlayerSettingDataKey, "{}"); // empty object
        }
        auto settings = nlohmann::json::parse(*mDB->get(DbPlayerSettingDataKey));
        if (!settings.is_object()) {
            throw std::runtime_error("player settings is not an object");
        }

        for (auto& [key, value] : settings.items()) {
            PlayerSettings settings_;
            json_util::json2structWithDiffPatch(value, settings_);
            mPlayerSettings.emplace(key, std::move(settings_));
        }
    }
    void _loadLands() {
        ll::coro::Generator<std::pair<std::string_view, std::string_view>> iter = mDB->iter();

        auto& landMigrator = internal::LandMigrator::getInstance();

        LandID safeId{0};
        for (auto [key, value] : iter) {
            if (!isLandData(key)) continue;

            auto json = nlohmann::json::parse(value);
            if (auto expected = landMigrator.migrate(json, LandSchemaVersion); !expected) {
                throw std::runtime_error{expected.error().message()};
            }

            auto land = Land::make();
            land->load(json);

            // 保证landID唯一
            if (safeId <= land->getId()) {
                safeId = land->getId() + 1;
            }

            mLandCache.emplace(land->getId(), std::move(land));
        }

        mLandIdAllocator = std::make_unique<internal::LandIdAllocator>(safeId); // 初始化ID分配器
    }
    void _loadLandTemplatePermTable() {
        if (!mDB->has(DbTemplatePermKey)) {
            auto t = LandPermTable{};
            mDB->set(DbTemplatePermKey, json_util::struct2json(t).dump());
        }

        auto rawJson = mDB->get(DbTemplatePermKey);
        try {
            auto json = nlohmann::json::parse(*rawJson);
            if (!json.is_object()) {
                throw std::runtime_error("Template perm table is not an object");
            }

            auto t = LandPermTable{};
            json_util::json2structWithDiffPatch(json, t); // 反射并补丁

            mLandTemplatePermTable = std::make_unique<LandTemplatePermTable>(t);
        } catch (...) {
            mLandTemplatePermTable = std::make_unique<LandTemplatePermTable>(LandPermTable{});
            PLand::getInstance().getSelf().getLogger().error(
                "Failed to load template perm table, using default perm table instead"
            );
        }
    }

    void _openDatabaseAndEnsureVersion() {
        auto&       self    = land::PLand::getInstance().getSelf();
        auto&       logger  = self.getLogger();
        auto const& dataDir = self.getDataDir();
        auto const  dbDir   = dataDir / DbDirName;

        bool const isNewCreatedDB = !std::filesystem::exists(dbDir); // 是否是新建的数据库

        auto backup = [&]() {
            auto const backupDir =
                dataDir
                / ("backup_db_" + std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()))
                );
            std::filesystem::copy(
                dbDir,
                backupDir,
                std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing
            );
        };

        if (!mDB) {
            mDB = std::make_unique<ll::data::KeyValueDB>(dbDir);
        }

        if (!mDB->has(DbVersionKey)) {
            if (isNewCreatedDB) {
                mDB->set(DbVersionKey, std::to_string(LandSchemaVersion)); // 设置初始版本号
            } else {
                mDB->set(DbVersionKey, "-1"); // 数据库存在，但没有版本号，表示是旧版数据库(0.8.1之前)
            }
        }

        auto version = std::stoi(*mDB->get(DbVersionKey));
        if (version != LandSchemaVersion) {
            if (version > LandSchemaVersion) {
                logger.fatal(
                    "The database version is too high, current version: {}, expected version: {}. In order to "
                    "keep the data safe, the plugin refuses to load!",
                    version,
                    LandSchemaVersion
                );
                throw std::runtime_error("The database versions do not match");

            } else if (version < LandSchemaVersion) {
                logger.warn(
                    "The database version is too low, the current version: {}, the expected version: {}, the "
                    "plugin will try to back up and upgrade the database...",
                    version,
                    LandSchemaVersion
                );
                mDB.reset();
                backup();
                mDB = std::make_unique<ll::data::KeyValueDB>(dbDir);
                mDB->set(DbVersionKey, std::to_string(LandSchemaVersion)); // 更新版本号
                // 这里只需要修改版本号以及备份，其它兼容转换操作将在 _checkVersionAndTryAdaptBreakingChanges 中进行
            }
        }
    }

    void _buildDimensionChunkMap() {
        for (auto& [id, land] : mLandCache) {
            mDimensionChunkMap.addLand(land);
        }
    }

    ll::Expected<> _addLand(SharedLand land, bool allocateId = true) {
        if (!land || (allocateId && land->getId() != INVALID_LAND_ID)) {
            return StorageError::make(StorageError::ErrorCode::InvalidLand, "The land is invalid or land ID is not -1");
        }
        if (allocateId) {
            land->_setLandId(mLandIdAllocator->nextId());
            land->markDirty();
        }
        std::unique_lock lock(mMutex);

        auto result = mLandCache.emplace(land->getId(), land);
        if (!result.second) {
            return StorageError::make(StorageError::ErrorCode::CacheMapError, "Failed to insert land into cache map");
        }

        mDimensionChunkMap.addLand(land);
        return {};
    }
    ll::Expected<> _removeLand(SharedLand const& ptr) {
        mDimensionChunkMap.removeLand(ptr);
        if (!mLandCache.erase(ptr->getId())) {
            mDimensionChunkMap.addLand(ptr);
            return StorageError::make(StorageError::ErrorCode::CacheMapError, "Failed to erase land from cache");
        }

        if (!this->mDB->del(std::to_string(ptr->getId()))) {
            mLandCache.emplace(ptr->getId(), ptr); // rollback
            mDimensionChunkMap.addLand(ptr);
            return StorageError::make(StorageError::ErrorCode::DatabaseError, "Failed to delete land from database");
        }
        return {};
    }

    bool _save(SharedLand const& land, bool force = false) const {
        if (!land->isDirty() || !force) {
            return true; // 没有变化，无需保存
        }
        if (mDB->set(std::to_string(land->getId()), land->toJson().dump())) {
            land->getDirtyCounter().reset();
            return true;
        }
        return false;
    }
};

LandID LandRegistry::_allocateNextId() { return impl->mLandIdAllocator->nextId(); }

LandRegistry::LandRegistry() : impl(std::make_unique<Impl>()) {
    auto& logger = land::PLand::getInstance().getSelf().getLogger();

    logger.trace("打开数据库...");
    impl->_openDatabaseAndEnsureVersion();

    auto lock = std::unique_lock<std::shared_mutex>(impl->mMutex);
    logger.trace("加载操作员...");
    impl->_loadOperators();
    logger.info("已加载 {} 位操作员", impl->mLandOperators.size());

    logger.trace("加载玩家设置...");
    impl->_loadPlayerSettings();
    logger.info("已加载 {} 位玩家的设置", impl->mPlayerSettings.size());

    logger.trace("加载领地数据...");
    impl->_loadLands();
    logger.info("已加载 {} 块领地数据", impl->mLandCache.size());

    logger.trace("加载模板权限表...");
    impl->_loadLandTemplatePermTable();
    logger.info("已加载模板权限表");

    logger.trace("构建维度区块映射...");
    impl->_buildDimensionChunkMap();
    logger.info("初始化维度区块映射完成");

    // TODO: 构建领地层级缓存

    lock.unlock();
    impl->mThread = std::thread([this]() {
        while (!impl->mThreadQuit) {
            std::unique_lock<std::mutex> lk(impl->mThreadMutex);
            if (impl->mThreadCV.wait_for(lk, std::chrono::minutes(2), [this] { return impl->mThreadQuit.load(); })) {
                break; // 被 stop 唤醒
            }
            lk.unlock();
            land::PLand::getInstance().getSelf().getLogger().debug("[Thread] Saving land data...");
            this->save();
            land::PLand::getInstance().getSelf().getLogger().debug("[Thread] Land data saved.");
        }
    });
}

LandRegistry::~LandRegistry() {
    impl->mThreadQuit = true;
    impl->mThreadCV.notify_all(); // 唤醒线程
    if (impl->mThread.joinable()) impl->mThread.join();
}


bool LandRegistry::isLandData(std::string_view key) {
    return key != DbVersionKey && key != DbOperatorDataKey && key != DbPlayerSettingDataKey && key != DbTemplatePermKey;
}

void LandRegistry::save() {
    std::shared_lock<std::shared_mutex> lock(impl->mMutex); // 获取锁
    impl->mDB->set(DbOperatorDataKey, json_util::struct2json(impl->mLandOperators).dump());

    impl->mDB->set(DbPlayerSettingDataKey, json_util::struct2json(impl->mPlayerSettings).dump());

    if (impl->mLandTemplatePermTable->isDirty()) {
        if (impl->mDB->set(DbTemplatePermKey, json_util::struct2json(impl->mLandTemplatePermTable->get()).dump())) {
            impl->mLandTemplatePermTable->resetDirty();
        }
    }

    for (auto const& land : impl->mLandCache | std::views::values) {
        (void)impl->_save(land, false);
    }
}

bool LandRegistry::save(std::shared_ptr<Land> const& land, bool force) const {
    std::unique_lock lock(impl->mMutex); // 获取锁
    return impl->_save(land, force);
}


bool LandRegistry::isOperator(mce::UUID const& uuid) const {
    std::shared_lock<std::shared_mutex> lock(impl->mMutex);
    return std::find(impl->mLandOperators.begin(), impl->mLandOperators.end(), uuid) != impl->mLandOperators.end();
}
bool LandRegistry::addOperator(mce::UUID const& uuid) {
    if (isOperator(uuid)) {
        return false;
    }
    std::unique_lock<std::shared_mutex> lock(impl->mMutex); // 获取锁
    impl->mLandOperators.push_back(uuid);
    return true;
}
bool LandRegistry::removeOperator(mce::UUID const& uuid) {
    std::unique_lock<std::shared_mutex> lock(impl->mMutex); // 获取锁

    auto iter = std::find(impl->mLandOperators.begin(), impl->mLandOperators.end(), uuid);
    if (iter == impl->mLandOperators.end()) {
        return false;
    }
    impl->mLandOperators.erase(iter);
    return true;
}
std::vector<mce::UUID> const& LandRegistry::getOperators() const {
    std::shared_lock<std::shared_mutex> lock(impl->mMutex);
    return impl->mLandOperators;
}


PlayerSettings* LandRegistry::getPlayerSettings(mce::UUID const& uuid) {
    std::shared_lock<std::shared_mutex> lock(impl->mMutex);
    auto                                iter = impl->mPlayerSettings.find(uuid);
    if (iter == impl->mPlayerSettings.end()) {
        return nullptr;
    }
    return &iter->second;
}
bool LandRegistry::setPlayerSettings(mce::UUID const& uuid, PlayerSettings settings) {
    std::unique_lock<std::shared_mutex> lock(impl->mMutex);
    impl->mPlayerSettings[uuid] = std::move(settings);
    return true;
}
bool LandRegistry::hasPlayerSettings(mce::UUID const& uuid) const {
    std::shared_lock<std::shared_mutex> lock(impl->mMutex);
    return impl->mPlayerSettings.find(uuid) != impl->mPlayerSettings.end();
}

LandTemplatePermTable& LandRegistry::getLandTemplatePermTable() const { return *impl->mLandTemplatePermTable; }

bool LandRegistry::hasLand(LandID id) const {
    std::shared_lock<std::shared_mutex> lock(impl->mMutex);
    return impl->mLandCache.find(id) != impl->mLandCache.end();
}

void LandRegistry::refreshLandRange(SharedLand const& ptr) {
    std::unique_lock<std::shared_mutex> lock(impl->mMutex);
    impl->mDimensionChunkMap.refreshRange(ptr);
}

ll::Expected<> LandRegistry::addOrdinaryLand(SharedLand const& land) {
    if (!land->isOrdinaryLand()) {
        return StorageError::make(
            StorageError::ErrorCode::LandTypeMismatch,
            "The land type does not match the required type"
        );
    }
    if (!LandCreateValidator::isLandRangeLegal(land->getAABB(), land->getDimensionId(), land->is3D())
        || !LandCreateValidator::isLandInForbiddenRange(land->getAABB(), land->getDimensionId())
        || !LandCreateValidator::isOrdinaryLandRangeConflict(*this, land)) {
        return StorageError::make(StorageError::ErrorCode::LandRangeIllegal, "The land range is illegal");
    }
    return impl->_addLand(land);
}
ll::Expected<> LandRegistry::removeOrdinaryLand(SharedLand const& ptr) {
    if (!ptr->isOrdinaryLand()) {
        return StorageError::make(
            StorageError::ErrorCode::LandTypeMismatch,
            "The land type does not match the required type"
        );
    }

    std::unique_lock lock(impl->mMutex); // 获取锁
    return impl->_removeLand(ptr);
}

ll::Expected<> LandRegistry::executeTransaction(
    std::unordered_set<std::shared_ptr<Land>> const& participants,
    TransactionCallback const&                       executor
) {
    std::unique_lock lock{impl->mMutex};

    struct Snapshot {
        LandContext context;
        int         dirtyCount;
    };
    std::unordered_map<Land*, Snapshot> snapshots;
    snapshots.reserve(participants.size());
    for (auto& land : participants) {
        snapshots[land.get()] = {land->_getContext(), land->getDirtyCounter().getCounter()};
    }

    TransactionContext ctx(*this);
    bool               success = false;
    try {
        success = executor(ctx);
    } catch (...) {
        success = false;
    }

    if (!success) {
        // === 回滚 (Rollback) ===
        for (auto& land : participants) {
            auto snapshot = snapshots[land.get()];
            land->_reinit(std::move(snapshot.context), snapshot.dirtyCount);
        }

        // 回滚 ID 分配器
        for (size_t i = 0; i < ctx.mAllocatedIds.size(); ++i) {
            impl->mLandIdAllocator->revertId();
        }
        return StorageError::make(StorageError::ErrorCode::TransactionError, "Transaction aborted.");
    }

    // === 提交 (Commit) ===
    for (auto& land : participants) {
        if (ctx.mLandsToRemove.contains(land->getId())) {
            if (auto res = impl->_removeLand(land); !res) {
                PLand::getInstance().getSelf().getLogger().error("Failed to remove land during commit!");
            }
            continue;
        }

        // 处理新增 (有 ID 但不在 Cache 里)
        // 如果是新建的 SubLand，它现在有了 ID，但还没进 mLandCache。

        bool justAllocated =
            std::find(ctx.mAllocatedIds.begin(), ctx.mAllocatedIds.end(), land->getId()) != ctx.mAllocatedIds.end();

        if (justAllocated) {
            // 新领地，直接入库
            // 注意：_addLand 内部不要再分配 ID 了，因为已经分过了
            if (auto res = impl->_addLand(land, false /* don't allocate id */); !res) {
                return res;
            }
        } else if (land->isDirty()) {
            (void)impl->_save(land, false);
        }
    }
    return {};
}

SharedLand LandRegistry::getLand(LandID id) const {
    std::shared_lock lock(impl->mMutex);

    auto landIt = impl->mLandCache.find(id);
    if (landIt != impl->mLandCache.end()) {
        return landIt->second;
    }
    return nullptr;
}
std::vector<SharedLand> LandRegistry::getLands() const {
    std::shared_lock lock(impl->mMutex);

    std::vector<SharedLand> lands;
    lands.reserve(impl->mLandCache.size());
    for (auto& land : impl->mLandCache) {
        lands.push_back(land.second);
    }
    return lands;
}
std::vector<SharedLand> LandRegistry::getLands(std::vector<LandID> const& ids) const {
    std::shared_lock lock(impl->mMutex);

    std::vector<SharedLand> lands;
    for (auto id : ids) {
        if (auto iter = impl->mLandCache.find(id); iter != impl->mLandCache.end()) {
            lands.push_back(iter->second);
        }
    }
    return lands;
}
std::vector<SharedLand> LandRegistry::getLands(LandDimid dimid) const {
    std::shared_lock lock(impl->mMutex);

    std::vector<SharedLand> lands;
    for (auto& land : impl->mLandCache) {
        if (land.second->getDimensionId() == dimid) {
            lands.push_back(land.second);
        }
    }
    return lands;
}
std::vector<SharedLand> LandRegistry::getLands(mce::UUID const& uuid, bool includeShared) const {
    std::shared_lock lock(impl->mMutex);

    std::vector<SharedLand> lands;
    for (auto& land : impl->mLandCache) {
        if (land.second->isOwner(uuid) || (includeShared && land.second->isMember(uuid))) {
            lands.push_back(land.second);
        }
    }
    return lands;
}
std::vector<SharedLand> LandRegistry::getLands(mce::UUID const& uuid, LandDimid dimid) const {
    std::shared_lock lock(impl->mMutex);

    std::vector<SharedLand> lands;
    for (auto& land : impl->mLandCache) {
        if (land.second->getDimensionId() == dimid && land.second->isOwner(uuid)) {
            lands.push_back(land.second);
        }
    }
    return lands;
}
std::unordered_map<mce::UUID, std::unordered_set<SharedLand>> LandRegistry::getLandsByOwner() const {
    std::shared_lock lock(impl->mMutex);

    std::unordered_map<mce::UUID, std::unordered_set<SharedLand>> lands;
    for (const auto& ptr : impl->mLandCache | std::views::values) {
        auto& owner = ptr->getOwner();
        lands[owner].insert(ptr);
    }
    return lands;
}
std::unordered_map<mce::UUID, std::unordered_set<SharedLand>> LandRegistry::getLandsByOwner(LandDimid dimid) const {
    std::shared_lock lock(impl->mMutex);

    std::unordered_map<mce::UUID, std::unordered_set<SharedLand>> res;
    for (const auto& ptr : impl->mLandCache | std::views::values) {
        if (ptr->getDimensionId() != dimid) {
            continue;
        }
        auto& owner = ptr->getOwner();
        res[owner].insert(ptr);
    }
    return res;
}


LandPermType LandRegistry::getPermType(mce::UUID const& uuid, LandID id, bool includeOperator) const {
    std::shared_lock lock(impl->mMutex);

    if (includeOperator && isOperator(uuid)) return LandPermType::Operator;

    if (auto land = getLand(id); land) {
        return land->getPermType(uuid);
    }

    return LandPermType::Guest;
}


SharedLand LandRegistry::getLandAt(BlockPos const& pos, LandDimid dimid) const {
    std::shared_lock<std::shared_mutex> lock(impl->mMutex);
    std::unordered_set<SharedLand>      result;

    auto landsIds = impl->mDimensionChunkMap.queryLand(dimid, EncodeChunkID(pos.x >> 4, pos.z >> 4));
    if (!landsIds) {
        return nullptr;
    }

    for (auto const& id : *landsIds) {
        if (auto iter = impl->mLandCache.find(id); iter != impl->mLandCache.end()) {
            if (auto const& land = iter->second; land->getAABB().hasPos(pos, land->is3D())) {
                result.insert(land);
            }
        }
    }

    if (!result.empty()) {
        if (result.size() == 1) {
            return *result.begin(); // 只有一个领地，即普通领地
        }

        // 子领地优先级最高
        SharedLand deepestLand = nullptr;
        int        maxLevel    = -1;
        for (auto& land : result) {
            int currentLevel = land->getNestedLevel();
            if (currentLevel > maxLevel) {
                maxLevel    = currentLevel;
                deepestLand = land;
            }
        }
        return deepestLand;
    }

    return nullptr;
}
std::unordered_set<SharedLand> LandRegistry::getLandAt(BlockPos const& center, int radius, LandDimid dimid) const {
    std::shared_lock<std::shared_mutex> lock(impl->mMutex);

    if (!impl->mDimensionChunkMap.hasDimension(dimid)) {
        return {};
    }

    std::unordered_set<ChunkID>    visitedChunks; // 记录已访问的区块
    std::unordered_set<SharedLand> lands;

    int minChunkX = (center.x - radius) >> 4;
    int minChunkZ = (center.z - radius) >> 4;
    int maxChunkX = (center.x + radius) >> 4;
    int maxChunkZ = (center.z + radius) >> 4;

    for (int x = minChunkX; x <= maxChunkX; ++x) {
        for (int z = minChunkZ; z <= maxChunkZ; ++z) {
            ChunkID chunkId = EncodeChunkID(x, z);
            if (visitedChunks.find(chunkId) != visitedChunks.end()) {
                continue; // 如果区块已经访问过，则跳过
            }
            visitedChunks.insert(chunkId);

            auto landsIds = impl->mDimensionChunkMap.queryLand(dimid, chunkId);
            if (!landsIds) {
                continue;
            }

            for (auto const& id : *landsIds) {
                if (auto iter = impl->mLandCache.find(id); iter != impl->mLandCache.end()) {
                    if (auto const& land = iter->second; land->isCollision(center, radius)) {
                        lands.insert(land);
                    }
                }
            }
        }
    }
    return lands;
}
std::unordered_set<SharedLand>
LandRegistry::getLandAt(BlockPos const& pos1, BlockPos const& pos2, LandDimid dimid) const {
    std::shared_lock<std::shared_mutex> lock(impl->mMutex);

    if (!impl->mDimensionChunkMap.hasDimension(dimid)) {
        return {};
    }

    std::unordered_set<ChunkID>    visitedChunks;
    std::unordered_set<SharedLand> lands;

    int minChunkX = std::min(pos1.x, pos2.x) >> 4;
    int minChunkZ = std::min(pos1.z, pos2.z) >> 4;
    int maxChunkX = std::max(pos1.x, pos2.x) >> 4;
    int maxChunkZ = std::max(pos1.z, pos2.z) >> 4;

    for (int x = minChunkX; x <= maxChunkX; ++x) {
        for (int z = minChunkZ; z <= maxChunkZ; ++z) {
            ChunkID chunkId = EncodeChunkID(x, z);
            if (visitedChunks.find(chunkId) != visitedChunks.end()) {
                continue;
            }
            visitedChunks.insert(chunkId);

            auto landsIds = impl->mDimensionChunkMap.queryLand(dimid, chunkId);
            if (!landsIds) {
                continue;
            }

            for (auto const& id : *landsIds) {
                if (auto iter = impl->mLandCache.find(id); iter != impl->mLandCache.end()) {
                    if (auto const& land = iter->second; land->isCollision(pos1, pos2)) {
                        lands.insert(land);
                    }
                }
            }
        }
    }
    return lands;
}

std::vector<SharedLand> LandRegistry::getLandsWhereRaw(ContextFilter const& filter) const {
    std::shared_lock<std::shared_mutex> lock(impl->mMutex);

    std::vector<SharedLand> result;
    for (auto const& [id, land] : impl->mLandCache) {
        if (filter(land->_getContext())) {
            result.push_back(land);
        }
    }
    return result;
}


} // namespace land


namespace land {
ChunkID LandRegistry::EncodeChunkID(int x, int z) {
    auto ux = static_cast<uint64_t>(std::abs(x));
    auto uz = static_cast<uint64_t>(std::abs(z));

    uint64_t signBits = 0;
    if (x >= 0) signBits |= (1ULL << 63);
    if (z >= 0) signBits |= (1ULL << 62);
    return signBits | (ux << 31) | (uz & 0x7FFFFFFF);
    // Memory layout:
    // [signBits][x][z] (signBits: 2 bits, x: 31 bits, z: 31 bits)
}
std::pair<int, int> LandRegistry::DecodeChunkID(ChunkID id) {
    bool xPositive = (id & (1ULL << 63)) != 0;
    bool zPositive = (id & (1ULL << 62)) != 0;

    int x = static_cast<int>((id >> 31) & 0x7FFFFFFF);
    int z = static_cast<int>(id & 0x7FFFFFFF);
    if (!xPositive) x = -x;
    if (!zPositive) z = -z;
    return {x, z};
}
} // namespace land
