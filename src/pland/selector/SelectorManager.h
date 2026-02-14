#pragma once
#include "ISelector.h"
#include "pland/Global.h"

#include "ll/api/coro/InterruptableSleep.h"
#include "ll/api/event/ListenerBase.h"

#include <concepts>
#include <unordered_map>


namespace land {
class Debouncer;

namespace service {
class SelectionService;
}

/**
 * @brief 选区管理器
 */
class SelectorManager final {
    struct Impl;
    std::unique_ptr<Impl> impl{nullptr};

    friend service::SelectionService;

    LDAPI bool _startSelection(std::unique_ptr<ISelector> selector);

public:
    LD_DISABLE_COPY_AND_MOVE(SelectorManager);

    explicit SelectorManager();
    ~SelectorManager();

    // 玩家是否正在选区 & 是否有选区任务
    LDNDAPI bool hasSelector(mce::UUID const& uuid) const;
    LDNDAPI bool hasSelector(Player& player) const;

    // 获取选区任务
    LDNDAPI ISelector* getSelector(mce::UUID const& uuid) const;
    LDNDAPI ISelector* getSelector(Player& player) const;

    // 开始选区
    template <typename T>
        requires std::derived_from<T, ISelector> && std::is_final_v<T>
    bool startSelection(std::unique_ptr<T> selector) {
        return _startSelection(std::move(selector));
    }

    // 停止选区
    LDAPI void stopSelection(mce::UUID const& uuid);
    LDAPI void stopSelection(Player& player);

    using ForEachFunc = std::function<bool(mce::UUID const&, ISelector*)>;
    LDAPI void forEach(ForEachFunc const& func) const;
};


} // namespace land