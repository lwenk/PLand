#pragma once
#include "pland/selector/ISelector.h"


namespace land {

class Land;

class LandResizeSelector final : public ISelector {
    std::weak_ptr<Land> mLand;           // 领地
    drawer::GeoId       mOldRangeDrawId; // 旧领地范围

public:
    LDAPI explicit LandResizeSelector(Player& player, std::shared_ptr<Land> land);
    LDAPI ~LandResizeSelector() override;

    LDNDAPI std::shared_ptr<Land> getLand() const;
};


} // namespace land