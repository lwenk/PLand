#pragma once
#include "pland/Global.h"
#include "pland/economy/EconomyConfig.h"

namespace land::economy {


class IEconomy {
public:
    LD_DISABLE_COPY_AND_MOVE(IEconomy);

    LDAPI virtual ~IEconomy() = default;

    LDAPI explicit IEconomy();

public:
    virtual llong get(Player& player) const        = 0;
    virtual llong get(mce::UUID const& uuid) const = 0;

    virtual bool set(Player& player, llong amount) const        = 0;
    virtual bool set(mce::UUID const& uuid, llong amount) const = 0;

    virtual bool add(Player& player, llong amount) const        = 0;
    virtual bool add(mce::UUID const& uuid, llong amount) const = 0;

    virtual bool reduce(Player& player, llong amount) const        = 0;
    virtual bool reduce(mce::UUID const& uuid, llong amount) const = 0;

    virtual bool transfer(Player& from, Player& to, llong amount) const                   = 0;
    virtual bool transfer(mce::UUID const& from, mce::UUID const& to, llong amount) const = 0;

    LDNDAPI virtual bool has(Player& player, llong amount) const;
    LDNDAPI virtual bool has(mce::UUID const& uuid, llong amount) const;

public:
    LDNDAPI virtual std::string getCostMessage(Player& player, llong amount) const;

    LDAPI virtual void sendNotEnoughMoneyMessage(Player& player, llong amount) const;

    LDNDAPI virtual EconomyConfig& getConfig() const;
};


} // namespace land::economy