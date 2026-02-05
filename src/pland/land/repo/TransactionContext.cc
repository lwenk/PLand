#include "TransactionContext.h"

#include "pland/land/Land.h"
#include "pland/land/repo/LandRegistry.h"

namespace land {

TransactionContext::TransactionContext(LandRegistry& landRegistry) : mLandRegistry(landRegistry) {}

void TransactionContext::allocateId(std::shared_ptr<Land> const& land) {
    if (land->getId() != INVALID_LAND_ID) {
        throw std::runtime_error("Land already has an id");
    }
    LandID id = mLandRegistry._allocateNextId();
    land->_setLandId(id);
    mAllocatedIds.push_back(id);
}

void TransactionContext::markForRemoval(std::shared_ptr<Land> const& land) {
    if (land->getId() != INVALID_LAND_ID) {
        mLandsToRemove.insert(land->getId());
    }
}

} // namespace land