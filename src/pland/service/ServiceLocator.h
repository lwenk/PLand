#pragma once
#include "pland/Global.h"

#include <memory>


namespace land {
class PLand;
namespace service {

class LandHierarchyService;
class LandManagementService;
class LandPriceService;
class SelectionService;

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

    SelectionService& getSelectionService() const;
};

} // namespace service
} // namespace land
