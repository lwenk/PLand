#pragma once
#include "pland/land/repo/LandContext.h"
#include <functional>

class Player;

namespace land {


class EditLandPermTableUtilGUI {
public:
    EditLandPermTableUtilGUI() = delete;

    using Callback = std::function<void(Player& player, LandPermTable newTable)>;
    static void sendTo(Player& player, LandPermTable const& table, Callback callback);
};


} // namespace land