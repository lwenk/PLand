#pragma once
#include "pland/Global.h"

namespace land::internal::interceptor {


struct IHookGuard {
    virtual ~IHookGuard() = default;
};

template <typename T>
concept Hookable = requires(T) {
    { T::hook() };
    { T::unhook() };
};

template <Hookable T>
struct HookGuardImpl final : IHookGuard {
    HookGuardImpl() { T::hook(); }
    ~HookGuardImpl() override { T::unhook(); }
};


} // namespace land::internal::interceptor