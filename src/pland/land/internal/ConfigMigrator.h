#pragma once
#include "pland/infra/migrator/JsonMigrator.h"

namespace land {
namespace internal {

class ConfigMigrator : public JsonMigrator {
public:
    ConfigMigrator();

    static ConfigMigrator& getInstance();
};

} // namespace internal
} // namespace land
