#include "PlayerRequestCreateLandEvent.h"

#include "Helper.h"


namespace land::event {


PlayerRequestCreateLandEvent::PlayerRequestCreateLandEvent(Player& player, Type type)
: PlayerEvent(player),
  mType(type) {}

PlayerRequestCreateLandEvent::Type event::PlayerRequestCreateLandEvent::type() const { return mType; }


IMPLEMENT_EVENT_EMITTER(PlayerRequestCreateLandEvent);


} // namespace land::event