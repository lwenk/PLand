#include "LandResizeSelector.h"
#include "mc/deps/core/math/Color.h"
#include "pland/PLand.h"
#include "pland/drawer/DrawHandleManager.h"
#include "pland/land/Land.h"
#include "pland/selector/ISelector.h"


namespace land {


LandResizeSelector::LandResizeSelector(Player& player, SharedLand land)
: ISelector(player, land->getDimensionId(), land->is3D()),
  mLand(land) {
    mOldRangeDrawId = PLand::getInstance().getDrawHandleManager()->getOrCreateHandle(player)->draw(
        land->getAABB(),
        land->getDimensionId(),
        mce::Color::BLUE()
    );
}

LandResizeSelector::~LandResizeSelector() {
    auto player = getPlayer();
    if (!player) {
        return;
    }

    if (mOldRangeDrawId) {
        PLand::getInstance().getDrawHandleManager()->getOrCreateHandle(*player)->remove(mOldRangeDrawId);
    }
}

SharedLand LandResizeSelector::getLand() const { return mLand.lock(); }


} // namespace land