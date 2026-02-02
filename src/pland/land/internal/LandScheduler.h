#pragma once
#include "pland/Global.h"

class Player;

namespace land {


/**
 * @brief 领地调度器
 * 处理玩家进出领地事件、以及标题提示等
 */
class LandScheduler {
    struct Impl;
    std::unique_ptr<Impl> impl{nullptr};

public:
    LD_DISABLE_COPY_AND_MOVE(LandScheduler);
    explicit LandScheduler();
    ~LandScheduler();
};


} // namespace land
