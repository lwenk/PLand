#pragma once
#include <cstdlib>
#include <utility>

namespace land::internal {

using ChunkID = uint64_t; // 区块ID

struct ChunkEncoder {
    ChunkEncoder() = delete;

    [[nodiscard]] inline static ChunkID encode(int x, int z) {
        auto ux = static_cast<uint64_t>(std::abs(x));
        auto uz = static_cast<uint64_t>(std::abs(z));

        uint64_t signBits = 0;
        if (x >= 0) signBits |= (1ULL << 63);
        if (z >= 0) signBits |= (1ULL << 62);
        return signBits | (ux << 31) | (uz & 0x7FFFFFFF);
        // Memory layout:
        // [signBits][x][z] (signBits: 2 bits, x: 31 bits, z: 31 bits)
    }

    [[nodiscard]] inline static std::pair<int, int> decode(ChunkID id) {
        bool xPositive = (id & (1ULL << 63)) != 0;
        bool zPositive = (id & (1ULL << 62)) != 0;

        int x = static_cast<int>((id >> 31) & 0x7FFFFFFF);
        int z = static_cast<int>(id & 0x7FFFFFFF);
        if (!xPositive) x = -x;
        if (!zPositive) z = -z;
        return {x, z};
    }
};


} // namespace land::internal