#include "SimpleInputForm.h"

#include <ll/api/form/CustomForm.h>

namespace land {
namespace gui {

using ll::form::CustomForm;

void SimpleInputForm::sendTo(
    Player&            player,
    std::string const& title,
    std::string const& text,
    std::string const& defaultVal,
    Callback           callback
) {
    CustomForm f{title};
    f.appendInput("str", text, "string", defaultVal);
    f.sendTo(
        player,
        [cb = std::move(callback)](Player& pl, ll::form::CustomFormResult res, ll::form::FormCancelReason) {
            if (!res.has_value()) {
                return;
            }
            cb(pl, std::get<std::string>(res->at("str")));
        }
    );
}


} // namespace gui
} // namespace land