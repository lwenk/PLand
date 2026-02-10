#pragma once
#include <functional>
#include <ll/api/form/SimpleForm.h>

class Player;

namespace land {
struct LandPermTable;
namespace gui {

struct PermTableEditor {
    PermTableEditor() = delete;

    using Callback = std::function<void(Player& player, LandPermTable newTable)>;

    static void
    sendTo(Player& player, LandPermTable const& table, Callback callback, ll::form::SimpleForm::ButtonCallback backTo);

private:
    struct Impl;
};

} // namespace gui
} // namespace land
