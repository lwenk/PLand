#include "ServiceLocator.h"

#include "LandHierarchyService.h"
#include "LandManagementService.h"
#include "LandPriceService.h"
#include "pland/PLand.h"

namespace land {
namespace service {

struct ServiceLocator::Impl {
    std::unique_ptr<LandHierarchyService>  mLandHierarchyService{nullptr};
    std::unique_ptr<LandManagementService> mLandManagementService{nullptr};
    std::unique_ptr<LandPriceService>      mLandPriceService{nullptr};

    void init(PLand& entry) {
        mLandHierarchyService  = std::make_unique<LandHierarchyService>(entry.getLandRegistry());
        mLandManagementService = std::make_unique<LandManagementService>(
            entry.getLandRegistry(),
            *entry.getSelectorManager(),
            *mLandHierarchyService
        );
        mLandPriceService = std::make_unique<LandPriceService>(*mLandHierarchyService);
    }
    void destroy() {
        mLandPriceService.reset();
        mLandManagementService.reset();
        mLandHierarchyService.reset();
    }
};

ServiceLocator::ServiceLocator(PLand& entry) : impl(std::make_unique<Impl>()) { impl->init(entry); }
ServiceLocator::~ServiceLocator() { impl->destroy(); }

LandManagementService& ServiceLocator::getLandManagementService() const { return *impl->mLandManagementService; }
LandHierarchyService&  ServiceLocator::getLandHierarchyService() const { return *impl->mLandHierarchyService; }
LandPriceService&      ServiceLocator::getLandPriceService() const { return *impl->mLandPriceService; }


} // namespace service
} // namespace land