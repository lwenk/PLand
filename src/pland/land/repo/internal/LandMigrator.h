#pragma once
#include "pland/infra/migrator/JsonMigrator.h"

namespace land {
struct LandContext;
}
namespace land {
namespace internal {

class LandMigrator : public JsonMigrator {
public:
    explicit LandMigrator();

    static LandMigrator& getInstance();
};

} // namespace internal
} // namespace land
