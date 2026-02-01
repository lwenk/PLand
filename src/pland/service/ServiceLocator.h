#pragma once
#include "pland/Global.h"

#include <memory>

namespace land::service {
class LandPriceService;
}
namespace land {
class PLand;
}
namespace land::service {
class LandHierarchyService;
class LandManagementService;
}

namespace land {
namespace service {

class ServiceLocator {
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    ServiceLocator(PLand& entry);
    ~ServiceLocator();

    LD_DISABLE_COPY_AND_MOVE(ServiceLocator);

    LandManagementService& getLandManagementService() const;

    LandHierarchyService& getLandHierarchyService() const;

    LandPriceService& getLandPriceService() const;
};

} // namespace service
} // namespace land
