#include "ServiceLocator.h"

#include "LandManagementService.h"
#include "pland/PLand.h"

namespace land {
namespace service {

struct ServiceLocator::Impl {
    std::unique_ptr<LandManagementService> landManagementService{nullptr};


    void init(PLand& entry) {
        landManagementService =
            std::make_unique<LandManagementService>(entry.getLandRegistry(), *entry.getSelectorManager());
    }
    void destroy() { landManagementService.reset(); }
};

ServiceLocator::ServiceLocator(PLand& entry) : impl(std::make_unique<Impl>()) { impl->init(entry); }
ServiceLocator::~ServiceLocator() { impl->destroy(); }

LandManagementService& ServiceLocator::getLandManagementService() const { return *impl->landManagementService; }


} // namespace service
} // namespace land