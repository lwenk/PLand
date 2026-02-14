#include "SimpleLandPicker.h"

#include "pland/gui/utils/BackUtils.h"
#include "pland/land/Land.h"

namespace land {
namespace gui {

void SimpleLandPicker::sendTo(
    Player&                              player,
    std::vector<std::shared_ptr<Land>>   data,
    Callback                             callback,
    ll::form::SimpleForm::ButtonCallback backTo
) {
    auto f = ll::form::SimpleForm{};

    auto localeCode = player.getLocaleCode();
    f.setTitle("[PLand] | 领地选择器"_trl(localeCode));

    if (backTo) {
        back_utils::injectBackButton(f, std::move(backTo));
    }

    for (auto& land : data) {
        f.appendButton(
            "{}\n维度: {} | ID: {}"_trl(localeCode, land->getName(), land->getDimensionId(), land->getId()),
            "textures/ui/icon_recipe_nature",
            "path",
            [weak = std::weak_ptr{land}, callback](Player& player) {
                if (auto land = weak.lock()) {
                    callback(player, land);
                }
            }
        );
    }

    f.sendTo(player);
}

} // namespace gui
} // namespace land