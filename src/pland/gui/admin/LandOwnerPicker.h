#pragma once
#include "pland/Global.h"

#include <functional>

#include <ll/api/form/SimpleForm.h>

class Player;
namespace mce {
class UUID;
}

namespace land {
namespace gui {

class LandOwnerPicker {
    struct Impl;

public:
    LandOwnerPicker() = delete;

    using Callback = std::function<void(Player&, mce::UUID)>;
    LDAPI static void sendTo(Player& player, Callback callback, ll::form::SimpleForm::ButtonCallback backTo);
};

} // namespace gui
} // namespace land
