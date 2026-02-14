#pragma once
#include "IHookGuard.h"
#include "pland/Global.h"

#include <ll/api/event/ListenerBase.h>

namespace land::internal::interceptor {

class EventInterceptor final {
    struct Impl;
    std::unique_ptr<Impl> impl;

public:
    LD_DISABLE_COPY_AND_MOVE(EventInterceptor);
    EventInterceptor();
    ~EventInterceptor();

    template <typename Factory>
    void registerListenerIf(bool cond, Factory&& factory) {
        if (cond) {
            _registerListener(factory());
        }
    }

    template <Hookable T>
    void registerHookIf(bool cond) {
        if (cond) {
            _registerHook(std::make_unique<HookGuardImpl<T>>());
        }
    }

private:
    void _registerListener(ll::event::ListenerPtr listener);
    void _registerHook(std::unique_ptr<IHookGuard> hookGuard);

    void setupLLPlayerListeners();
    void setupLLEntityListeners();
    void setupLLWorldListeners();

    void setupIlaPlayerListeners();
    void setupIlaEntityListeners();
    void setupIlaWorldListeners();

    void setupHooks();
};

} // namespace land::internal::interceptor
