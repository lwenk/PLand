#pragma once
#include <functional>
#include <memory>

#include <ll/api/form/SimpleForm.h>

class Player;

namespace land {
class Land;
namespace gui {

struct SimpleLandPicker {
    SimpleLandPicker() = delete;

    using Callback = std::function<void(Player&, std::shared_ptr<Land> land)>;
    static void sendTo(
        Player&                              player,
        std::vector<std::shared_ptr<Land>>   data,
        Callback                             callback,
        ll::form::SimpleForm::ButtonCallback backTo = nullptr
    );
};

} // namespace gui
} // namespace land
