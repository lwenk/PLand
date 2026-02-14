#include "LandPriceService.h"

#include "LandHierarchyService.h"
#include "pland/economy/PriceCalculate.h"
#include "pland/land/Config.h"
#include "pland/land/Land.h"

namespace land {
namespace service {

struct LandPriceService::Impl {
    LandHierarchyService& mHierarchyService;
};

LandPriceService::LandPriceService(LandHierarchyService& service) : impl(std::make_unique<Impl>(service)) {}
LandPriceService::~LandPriceService() = default;

int64_t LandPriceService::calculatePriceRecursively(
    std::shared_ptr<Land>                                              land,
    std::function<bool(std::shared_ptr<Land> const&, int64_t&)> const& func
) const {
    int64_t result = 0;

    auto iter = impl->mHierarchyService.getDescendants(land);
    for (auto i : iter) {
        if (!func) {
            result += i->getOriginalBuyPrice();
        } else {
            if (!func(i, result)) {
                break;
            }
        }
    }
    return result;
}
ll::Expected<LandPriceService::PriceResult>
LandPriceService::getOrdinaryLandPrice(LandAABB const& range, int dimId, bool is3D) const {
    return _getLandPrice(range, dimId, Config::getLandPriceCalculateFormula(is3D));
}
ll::Expected<LandPriceService::PriceResult> LandPriceService::getSubLandPrice(LandAABB const& range, int dimId) const {
    return _getLandPrice(range, dimId, Config::getSubLandPriceCalculateFormula());
}
int64_t LandPriceService::getRefundAmount(std::shared_ptr<Land> const& land) const {
    return PriceCalculate::calculateRefundsPrice(land->getOriginalBuyPrice(), Config::cfg.land.refundRate);
}
int64_t LandPriceService::getRefundAmountRecursively(std::shared_ptr<Land> const& land) const {
    return calculatePriceRecursively(land, [](std::shared_ptr<Land> const& land, int64_t& price) {
        price += PriceCalculate::calculateRefundsPrice(land->getOriginalBuyPrice(), Config::cfg.land.refundRate);
        return true;
    });
}

ll::Expected<LandPriceService::PriceResult>
LandPriceService::_getLandPrice(LandAABB const& range, int dimId, std::string const& calculateFormula) const {
    if (!Config::ensureEconomySystemEnabled()) {
        return ll::makeStringError("Economy system is not enabled");
    }
    auto variable = PriceCalculate::Variable::make(range, dimId);
    auto expected = PriceCalculate::eval(calculateFormula, variable);
    if (!expected) return ll::makeStringError(expected.error().message());

    auto originalPrice = expected.value();
    auto multipliers   = Config::getLandDimensionMultipliers(dimId);
    if (multipliers) {
        originalPrice *= *multipliers;
    }
    auto discountedPrice = PriceCalculate::calculateDiscountPrice(originalPrice, Config::cfg.land.discountRate);
    return PriceResult{static_cast<long long>(originalPrice), discountedPrice, multipliers};
}


} // namespace service
} // namespace land