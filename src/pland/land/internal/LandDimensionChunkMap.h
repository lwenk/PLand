#pragma once
#include "BidirectionalMap.h"
#include "pland/Global.h"

#include <unordered_map>

namespace land {
class Land;
}

namespace land::internal {


/**
 * @brief 领地维度区块双向映射表
 *         / --> 区块 --> [领地]  # 查询领地
 * 维度 --|
 *        \ --> 领地 --> [区块]   # 查询区块
 */
class LandDimensionChunkMap {
public:
    using Map = std::unordered_map<LandDimid, BidirectionalMap<ChunkID, LandID>>;

public:
    LandDimensionChunkMap();

    /**
     * @brief 查询维度是否存在
     */
    [[nodiscard]] bool hasDimension(LandDimid dimId) const;

    /**
     * @brief 查询区块是否存在
     */
    [[nodiscard]] bool hasChunk(LandDimid dimId, ChunkID chunkId) const;

    /**
     * @brief 查询领地是否存在
     */
    [[nodiscard]] bool hasLand(LandDimid dimId, LandID landId) const;

    /**
     * @brief 查询某个区块下所有的领地
     */
    [[nodiscard]] std::unordered_set<LandID> const* queryLand(LandDimid dimId, ChunkID chunkId) const;

    /**
     * @brief 查询某个领地下所有的区块
     */
    [[nodiscard]] std::unordered_set<ChunkID> const* queryChunk(LandDimid dimId, LandID landId) const;

    void addLand(std::shared_ptr<Land> const& land);

    void removeLand(std::shared_ptr<Land> const& land);

    void refreshRange(std::shared_ptr<Land> const& land);

private:
    Map mMap;
};

} // namespace land::internal
