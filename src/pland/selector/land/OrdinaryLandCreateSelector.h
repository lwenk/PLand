#pragma once
#include "pland/selector/ISelector.h"


namespace land {

class Land;

class OrdinaryLandCreateSelector final : public ISelector {
public:
    LDAPI explicit OrdinaryLandCreateSelector(Player& player, bool is3D);

    LDNDAPI std::shared_ptr<Land> newLand() const;
};


} // namespace land