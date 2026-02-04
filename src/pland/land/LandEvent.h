#pragma once
#include "ll/api/event/Cancellable.h"
#include "ll/api/event/Event.h"
#include "mc/world/actor/player/Player.h"
#include "pland/Global.h"
#include "pland/aabb/LandAABB.h"
#include "pland/land/Land.h"
#include "pland/selector/ISelector.h"
#include "pland/selector/SelectorManager.h"


namespace land {


// 领地成员变动(EditLandMemberGui)
class [[deprecated("Waiting for reconstruction")]] LandMemberChangeBeforeEvent final
: public ll::event::Cancellable<ll::event::Event> {
protected:
    Player&          mPlayer;       // 操作者
    mce::UUID const& mTargetPlayer; // 目标玩家
    LandID           mLandID;
    bool             mIsAdd; // true: 添加成员, false: 删除成员

public:
    LDAPI LandMemberChangeBeforeEvent(Player& player, mce::UUID const& targetPlayer, LandID landID, bool isAdd)
    : Cancellable(),
      mPlayer(player),
      mTargetPlayer(targetPlayer),
      mLandID(landID),
      mIsAdd(isAdd) {}

    LDNDAPI Player& getPlayer() const;
    LDNDAPI mce::UUID const& getTargetPlayer() const;
    LDNDAPI LandID           getLandID() const;
    LDNDAPI bool             isAdd() const;
};
class [[deprecated("Waiting for reconstruction")]] LandMemberChangeAfterEvent final : public ll::event::Event {
protected:
    Player&          mPlayer;       // 操作者
    mce::UUID const& mTargetPlayer; // 目标玩家
    LandID           mLandID;
    bool             mIsAdd; // true: 添加成员, false: 删除成员

public:
    LDAPI LandMemberChangeAfterEvent(Player& player, mce::UUID const& targetPlayer, LandID landID, bool isAdd)
    : mPlayer(player),
      mTargetPlayer(targetPlayer),
      mLandID(landID),
      mIsAdd(isAdd) {}

    LDNDAPI Player& getPlayer() const;
    LDNDAPI mce::UUID const& getTargetPlayer() const;
    LDNDAPI LandID           getLandID() const;
    LDNDAPI bool             isAdd() const;
};


} // namespace land
