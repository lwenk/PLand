#include "LandMigrator.h"

#include <nlohmann/json.hpp>

namespace land {
namespace internal {

LandMigrator& LandMigrator::getInstance() {
    static LandMigrator instance;
    return instance;
}

LandMigrator::LandMigrator() {
    registerMigrator(15, [](nlohmann::json& data) -> ll::Expected<> {
        static constexpr std::string_view LegacyMaxKey = "mMax_B";
        static constexpr std::string_view LegacyMinKey = "mMin_A";
        static constexpr std::string_view NewMaxKey    = "max";
        static constexpr std::string_view NewMinKey    = "min";

        auto& pos = data.at("mPos");
        if (pos.contains(LegacyMaxKey)) {
            auto legacyMax = pos[LegacyMaxKey]; // copy
            pos.erase(LegacyMaxKey);
            pos[NewMaxKey] = std::move(legacyMax);
        }
        if (pos.contains(LegacyMinKey)) {
            auto legacyMin = pos[LegacyMinKey]; // copy
            pos.erase(LegacyMinKey);
            pos[NewMinKey] = std::move(legacyMin);
        }
        return {};
    });
}

} // namespace internal
} // namespace land