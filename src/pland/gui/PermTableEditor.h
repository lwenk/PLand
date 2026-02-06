#pragma once
#include <functional>

class Player;

namespace land {
struct LandPermTable;
namespace gui {

struct PermTableEditor {
    PermTableEditor() = delete;

    using Callback = std::function<void(Player& player, LandPermTable newTable)>;
    static void sendTo(Player& player, LandPermTable const& table, Callback callback);
};

} // namespace gui
} // namespace land
