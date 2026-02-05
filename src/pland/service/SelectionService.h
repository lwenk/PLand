#pragma once
#include "ll/api/Expected.h"

#include <memory>

class Player;

namespace land {
class SelectorManager;
class ISelector;
} // namespace land

namespace land::service {


class SelectionService {
    struct Impl;
    std::unique_ptr<Impl> impl;

    ll::Expected<> _beginSelection(Player& player, std::unique_ptr<ISelector> selector);

public:
    explicit SelectionService(SelectorManager& manager);
    ~SelectionService();

    /**
     * @return 玩家是否有正在进行中的选区任务
     */
    bool hasActiveSelection(Player& player) const;

    /**
     * 开始一个新的选区任务
     */
    template <typename T>
        requires std::derived_from<T, ISelector> && std::is_final_v<T>
    ll::Expected<> beginSelection(Player& player, std::unique_ptr<T> selector) {
        return _beginSelection(player, std::move(selector));
    }

    /**
     * 结束当前选区任务
     */
    void endSelection(Player& player);

    /**
     * 获取当前选区任务
     */
    ISelector* tryGetSelection(Player& player) const;
};


} // namespace land::service