#pragma once
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

    explicit PaginatedForm(Options options = Options{});
    ~PaginatedForm();

    PaginatedForm& setTitle(std::string title);

    PaginatedForm& setContent(std::string content);

    PaginatedForm&
    appendButton(std::string text, std::string imageData, std::string imageType, ButtonCallback callback = {});

    PaginatedForm& appendButton(std::string text, ButtonCallback callback = {});

    void sendTo(Player& player);
};

} // namespace gui
} // namespace land
