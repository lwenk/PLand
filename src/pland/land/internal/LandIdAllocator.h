#pragma once

namespace land::internal {


class LandIdAllocator {
    std::atomic<LandID> mCurrentId;

public:
    explicit LandIdAllocator(LandID currentId = 0) : mCurrentId(currentId) {}

    inline LandID nextId() { return mCurrentId.fetch_add(1, std::memory_order_relaxed); }

    inline void revertId() { mCurrentId.fetch_sub(1, std::memory_order_relaxed); }
};


} // namespace land::internal