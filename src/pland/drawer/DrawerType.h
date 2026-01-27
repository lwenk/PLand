#pragma once
#include <cstdint>

namespace land {


enum class DrawerType : uint8_t {
    DefaultParticle = 0, // 默认粒子系统
    DebugShape      = 1  // DebugShape 调试形状
};


}