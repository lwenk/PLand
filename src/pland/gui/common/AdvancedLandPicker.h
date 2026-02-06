#pragma once
#include "pland/Global.h"

#include <functional>
#include <memory>

#include <ll/api/form/SimpleForm.h>

namespace land {
class Land;
namespace gui {

class AdvancedLandPicker {
    struct Impl;

public:
    using Callback = std::function<void(Player&, std::shared_ptr<Land> land)>;
    LDAPI static void sendTo(
        Player&                              player,
        std::vector<std::shared_ptr<Land>>   data,
        Callback                             callback,
        ll::form::SimpleForm::ButtonCallback backTo = nullptr
    );
};

} // namespace gui
} // namespace land
