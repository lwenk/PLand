#include "EditLandPermTableUtilGUI.h"
#include "ll/api/form/CustomForm.h"
#include "pland/land/LandContext.h"
#include "pland/utils/JsonUtil.h"

namespace land {


void EditLandPermTableUtilGUI::sendTo(Player& player, const LandPermTable& table, Callback callback) {
    ll::form::CustomForm fm(PLUGIN_NAME + " | 编辑权限"_trf(player));

    auto& i18n       = ll::i18n::getInstance();
    auto  localeCode = GetPlayerLocaleCodeFromSettings(player);

    auto json = json_util::struct2json(table);
    for (auto& [k, v] : json.items()) {
        fm.appendToggle(k, (std::string)i18n.get(k, localeCode), v);
    }

    fm.sendTo(
        player,
        [cb = std::move(callback), json = std::move(json)](Player& pl, ll::form::CustomFormResult const& res, auto) {
            if (!res) {
                return;
            }

            auto copy = json;
            for (auto const& [key, value] : copy.items()) {
                bool const val = std::get<uint64_t>(res->at(key));
                copy[key]      = val;
            }

            LandPermTable obj{};
            json_util::json2struct(copy, obj);

            cb(pl, obj);
        }
    );
}


} // namespace land