#include "EventInterceptor.h"

#include <ll/api/event/EventBus.h>

namespace land::internal::interceptor {

struct EventInterceptor::Impl {
    std::vector<ll::event::ListenerPtr>      mListeners;
    std::vector<std::unique_ptr<IHookGuard>> mHookGuards;
};

EventInterceptor::EventInterceptor() : impl(std::make_unique<Impl>()) {
    setupLLPlayerListeners();
    setupLLEntityListeners();
    setupLLWorldListeners();
    setupIlaPlayerListeners();
    setupIlaEntityListeners();
    setupIlaWorldListeners();
    setupHooks();
}
EventInterceptor::~EventInterceptor() {
    auto& bus = ll::event::EventBus::getInstance();
    for (auto& listener : impl->mListeners) {
        bus.removeListener(listener);
    }
    impl->mListeners.clear();
    impl->mHookGuards.clear(); // RAII
}

void EventInterceptor::_registerListener(ll::event::ListenerPtr listener) {
    if (!listener) return;
    impl->mListeners.emplace_back(std::move(listener));
}
void EventInterceptor::_registerHook(std::unique_ptr<IHookGuard> hookGuard) {
    if (!hookGuard) return;
    impl->mHookGuards.push_back(std::move(hookGuard));
}

} // namespace land::internal::interceptor