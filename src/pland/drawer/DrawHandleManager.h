#pragma once
#include "IDrawerHandle.h"
#include "pland/Global.h"

#include <complex.h>
#include <memory>
#include <unordered_map>

class Player;

namespace land {

class DrawHandleManager final {
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    LD_DISABLE_COPY_AND_MOVE(DrawHandleManager);
    explicit DrawHandleManager();
    ~DrawHandleManager();

public:
    LDNDAPI drawer::IDrawerHandle* getOrCreateHandle(Player& player);

    LDNDAPI drawer::IDrawerHandle* tryGetHandle(mce::UUID const& uuid);

    LDAPI void removeHandle(Player& player);
    LDAPI void removeHandle(mce::UUID const& uuid);

    LDAPI void removeAllHandle();
};


} // namespace land