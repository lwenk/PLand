#include "IDrawerHandle.h"

#include "mc/world/actor/player/Player.h"

namespace land::drawer {

IDrawerHandle::IDrawerHandle()  = default;
IDrawerHandle::~IDrawerHandle() = default;

void IDrawerHandle::setTargetPlayer(Player& player) { mTargetPlayer = player.getWeakEntity(); }

optional_ref<Player> IDrawerHandle::getTargetPlayer() const { return mTargetPlayer.tryUnwrap<Player>(); }

} // namespace land::drawer