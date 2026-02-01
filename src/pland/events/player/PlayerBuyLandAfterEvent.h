#pragma once
#include <ll/api/event/player/PlayerEvent.h>

namespace land {
class Land;
}
namespace land {
namespace event {

class PlayerBuyLandAfterEvent final : public ll::event::PlayerEvent {
    std::shared_ptr<Land> mLand;
    int64_t               mPayMoney;

public:
    explicit PlayerBuyLandAfterEvent(Player& player, const std::shared_ptr<Land>& land, int64_t payMoney);

    std::shared_ptr<Land> land() const;

    int64_t payMoney() const;
};

} // namespace event
} // namespace land
