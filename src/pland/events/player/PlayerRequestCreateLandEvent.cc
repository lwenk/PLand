#include "PlayerRequestCreateLandEvent.h"

#include "pland/events/Helper.h"


namespace land::event {


PlayerRequestCreateLandEvent::PlayerRequestCreateLandEvent(Player& player, LandType type)
: PlayerEvent(player),
  mType(type) {}

LandType PlayerRequestCreateLandEvent::type() const { return mType; }


IMPLEMENT_EVENT_EMITTER(PlayerRequestCreateLandEvent);


} // namespace land::event