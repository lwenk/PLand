#pragma once
#include "pland/Global.h"


#include <functional>
#include <string>

class Player;

namespace land {
namespace gui {

struct SimpleInputForm {
    SimpleInputForm() = delete;

    using Callback = std::function<void(Player& player, std::string data)>;

    LDAPI static void sendTo(
        Player&            player,
        std::string const& title,
        std::string const& text,
        std::string const& defaultVal,
        Callback           callback
    );
};

} // namespace gui
} // namespace land
