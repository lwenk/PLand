#pragma once
#include "pland/Global.h"

namespace land {
class Land;

class TransactionContext {
    friend class LandRegistry;

    LandRegistry&              mLandRegistry;
    std::vector<LandID>        mAllocatedIds;
    std::unordered_set<LandID> mLandsToRemove;

public:
    explicit TransactionContext(LandRegistry& landRegistry);

    LD_DISABLE_COPY_AND_MOVE(TransactionContext);

    void allocateId(std::shared_ptr<Land> const& land);

    void markForRemoval(std::shared_ptr<Land> const& land);
};

} // namespace land
