#pragma once
#include "pland/Global.h"

namespace land {
class SelectorManager;
}
namespace land {
class LandRegistry;
}
namespace land {
namespace service {

class LandManagementService {
    struct Impl;
    std::unique_ptr<Impl> impl_;

public:
    LandManagementService(LandRegistry& registry, SelectorManager& selectorManager);
    ~LandManagementService();

    LD_DISABLE_COPY_AND_MOVE(LandManagementService);


    ll::Expected<> requestCreateOrdinaryLand(Player& player, bool is3D) const;

    ll::Expected<> requestCreateSubLand(Player& player);

private:
    static bool ensureDimensionAllowed(Player& player);

    static bool ensureSubLandFeatureEnabled();

    static bool ensureOrdinaryLandEnabled(bool is3D);
};

} // namespace service
} // namespace land
