#pragma once
#include "pland/selector/ISelector.h"


namespace land {

class Land;

class SubLandCreateSelector final : public ISelector {
    std::weak_ptr<Land> mParentLand;
    drawer::GeoId               mParentRangeDrawId;

public:
    LDAPI explicit SubLandCreateSelector(Player& player, std::shared_ptr<Land> parent);
    LDAPI ~SubLandCreateSelector() override;

    LDNDAPI std::shared_ptr<Land> getParentLand() const;

    LDNDAPI std::shared_ptr<Land> newSubLand() const;
};


} // namespace land