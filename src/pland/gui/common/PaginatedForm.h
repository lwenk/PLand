#pragma once
#include "pland/Global.h"

#include <functional>
#include <memory>
#include <string>

class Player;

namespace land {
namespace gui {

class PaginatedForm final {
    struct Impl;
    std::shared_ptr<Impl> impl;

public:
    struct Options {
        int  perPageButtons{32};          // 每页按钮数量
        bool enableJumpFirstOrLast{true}; // 是否启用跳转到第一页和最后一页按钮
        bool enableJumpSpecial{true};     // 是否启用跳转到指定页按钮
        Options();
    };

    using ButtonCallback = std::function<void(Player& player)>;

    LDAPI explicit PaginatedForm(Options options = Options{});
    LDAPI ~PaginatedForm();

    LDAPI PaginatedForm& setTitle(std::string title);

    LDAPI PaginatedForm& setContent(std::string content);

    LDAPI PaginatedForm&
    appendButton(std::string text, std::string imageData, std::string imageType, ButtonCallback callback = {});

    LDAPI PaginatedForm& appendButton(std::string text, ButtonCallback callback = {});

    LDAPI void sendTo(Player& player);
};

} // namespace gui
} // namespace land
