#pragma once
#include "ll-bstats/Telemetry.h"

namespace land {
namespace adapter {

class Telemetry : public ll_bstats::Telemetry {
public:
    Telemetry();

    void initConstant() override;

    void collect() override;
};

} // namespace adapter
} // namespace land
