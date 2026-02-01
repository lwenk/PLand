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


// 玩家 进入/离开 领地(LandScheduler)
class [[deprecated("Waiting for reconstruction")]] PlayerEnterLandEvent final : public ll::event::Event {
protected:
    Player& mPlayer;
    LandID  mLandID;

public:
    LDAPI constexpr explicit PlayerEnterLandEvent(Player& player, LandID landID) : mPlayer(player), mLandID(landID) {}

    LDNDAPI Player& getPlayer() const;
    LDNDAPI LandID  getLandID() const;
};
class [[deprecated("Waiting for reconstruction")]] PlayerLeaveLandEvent final : public ll::event::Event {
protected:
    Player& mPlayer;
    LandID  mLandID;

public:
    LDAPI constexpr explicit PlayerLeaveLandEvent(Player& player, LandID landID) : mPlayer(player), mLandID(landID) {}

    LDNDAPI Player& getPlayer() const;
    LDNDAPI LandID  getLandID() const;
};


// 玩家删除领地 (DeleteLandGui)
class [[deprecated("Waiting for reconstruction")]] PlayerDeleteLandBeforeEvent final
: public ll::event::Cancellable<ll::event::Event> {
protected:
    Player&    mPlayer;
    LandID     mLandID;
    int const& mRefundPrice; // 删除后返还的金额

public:
    LDAPI constexpr explicit PlayerDeleteLandBeforeEvent(Player& player, LandID landID, int const& refundPrice)
    : Cancellable(),
      mPlayer(player),
      mLandID(landID),
      mRefundPrice(refundPrice) {}

    LDNDAPI Player&    getPlayer() const;
    LDNDAPI LandID     getLandID() const;
    LDNDAPI int const& getRefundPrice() const;
};
class [[deprecated("Waiting for reconstruction")]] PlayerDeleteLandAfterEvent final : public ll::event::Event {
protected:
    Player& mPlayer;
    LandID  mLandID;

public:
    LDAPI constexpr explicit PlayerDeleteLandAfterEvent(Player& player, LandID landID)
    : mPlayer(player),
      mLandID(landID) {}

    LDNDAPI Player& getPlayer() const;
    LDNDAPI LandID  getLandID() const;
};


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


// 领地主人变动(EditLandOwnerGui)
class [[deprecated("Waiting for reconstruction")]] LandOwnerChangeBeforeEvent final
: public ll::event::Cancellable<ll::event::Event> {
protected:
    Player&          mPlayer;   // 操作者
    mce::UUID const& mNewOwner; // 新主人 UUID
    LandID           mLandID;

public:
    LDAPI LandOwnerChangeBeforeEvent(Player& player, mce::UUID const& newOwner, LandID landID)
    : Cancellable(),
      mPlayer(player),
      mNewOwner(newOwner),
      mLandID(landID) {}

    LDNDAPI Player& getPlayer() const;
    LDNDAPI mce::UUID const& getNewOwner() const;
    LDNDAPI LandID           getLandID() const;
};
class [[deprecated("Waiting for reconstruction")]] LandOwnerChangeAfterEvent final : public ll::event::Event {
protected:
    Player&          mPlayer;   // 操作者
    mce::UUID const& mNewOwner; // 目标玩家 UUID
    LandID           mLandID;

public:
    LDAPI LandOwnerChangeAfterEvent(Player& player, mce::UUID const& newOwner, LandID landID)
    : mPlayer(player),
      mNewOwner(newOwner),
      mLandID(landID) {}

    LDNDAPI Player& getPlayer() const;
    LDNDAPI mce::UUID const& getNewOwner() const;
    LDNDAPI LandID           getLandID() const;
};


} // namespace land
