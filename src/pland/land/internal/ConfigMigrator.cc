#include "ConfigMigrator.h"

#include <nlohmann/json.hpp>

namespace land {
namespace internal {

ConfigMigrator::ConfigMigrator() {
    registerMigrator(32, [](nlohmann::json& json) -> ll::Expected<> {
        if (json.contains("land")) {
            auto& land = json["land"];
            if (land.contains("subLand")) {
                auto& subLand = land["subLand"];
                if (subLand.contains("minSpacingIncludeY")) {
                    auto& minSpacingIncludeY = subLand["minSpacingIncludeY"];
                    if (minSpacingIncludeY.is_number_integer()) {
                        // fix(land): correct minSpacingIncludeY type from int to bool
                        // https://github.com/IceBlcokMC/PLand/pull/183/changes/aeb1d99cc15723ad10f8ea687091aa4f48001d48
                        bool val           = minSpacingIncludeY.get<int>();
                        minSpacingIncludeY = val;
                    }
                }
            }
        }
        return {};
    });
}

ConfigMigrator& ConfigMigrator::getInstance() {
    static ConfigMigrator instance;
    return instance;
}

} // namespace internal
} // namespace land