#pragma once
#include <cstdint>


namespace land {

struct LandResizeSettlement {
    int64_t newTotalPrice; // 新范围对应的总价

    enum class Type {
        NoChange, // 不补不退
        Pay,      // 需补差价
        Refund    // 需退还差价
    } type;

    int64_t amount; // 差额（始终为正）
};

} // namespace land