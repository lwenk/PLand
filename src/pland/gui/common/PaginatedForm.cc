#include "PaginatedForm.h"
#include "pland/Global.h"

#include <ll/api/form/SimpleForm.h>

#include <string>

namespace land {
namespace gui {

using ll::form::SimpleForm;

struct PaginatedForm::Impl : std::enable_shared_from_this<Impl> {
    struct ButtonData {
        std::string    text;
        std::string    imageData;
        std::string    imageType;
        ButtonCallback callback;
    };

    Options                 mOptions;
    std::string             mTitle;
    std::string             mContent;
    std::vector<ButtonData> mButtons;

    int  mTotalPages{0};
    int  mCurrentPage{1};
    bool mDirty{true};

    void countTotalPages() {
        mTotalPages = mButtons.size() / mOptions.perPageButtons;
        if (mButtons.size() % mOptions.perPageButtons != 0) {
            mTotalPages++;
        }
    }

    void _buildTopControlButtons(SimpleForm& form, std::string const& localeCode) {
        if (mOptions.enableJumpFirstOrLast) {
            form.appendButton(
                "首页"_trl(localeCode),
                "textures/ui/book_shiftleft_hover",
                "path",
                [data = shared_from_this()](Player& player) { data->sendFirstPage(player); }
            );
        }
        form.appendButton(
            "上一页"_trl(localeCode),
            "textures/ui/book_pageleft_default",
            "path",
            [data = shared_from_this()](Player& player) { data->sendPreviousPage(player); }
        );
    }

    void _buildBottomControlButtons(SimpleForm& form, std::string const& localeCode) {
        if (mOptions.enableJumpSpecial) {
            form.appendButton(
                "选择页"_trl(localeCode),
                "textures/ui/mashup_PaintBrush",
                "path",
                [data = shared_from_this()](Player& player) {
                    // TODO
                }
            );
        }
        form.appendButton(
            "下一页"_trl(localeCode),
            "textures/ui/book_pageright_default",
            "path",
            [data = shared_from_this()](Player& player) { data->sendNextPage(player); }
        );
        if (mOptions.enableJumpFirstOrLast) {
            form.appendButton(
                "末页"_trl(localeCode),
                "textures/ui/book_shiftright_hover",
                "path",
                [data = shared_from_this()](Player& player) { data->sendLastPage(player); }
            );
        }
    }

    int getPageElementCount(int page) {
        if (page < 1 || page > mTotalPages) {
            return 0;
        }
        if (page == mTotalPages) {
            auto rem = mButtons.size() % mOptions.perPageButtons;
            return rem == 0 ? mOptions.perPageButtons : rem;
        }
        return mOptions.perPageButtons;
    }

    void _buildPageButton(SimpleForm& form, int page) {
        auto count = getPageElementCount(page);

        auto baseAddr = (page - 1) * mOptions.perPageButtons;
        for (int i = 0; i < count; i++) {
            int   globalIndex = baseAddr + i;
            auto& button      = mButtons[globalIndex];
            form.appendButton(
                button.text,
                button.imageData,
                button.imageType,
                [globalIndex, data = shared_from_this()](Player& player) {
                    data->mButtons.at(globalIndex).callback(player);
                }
            );
        }
    }

    void buildPage(SimpleForm& form, int page, std::string const& localeCode) {
        form.setTitle(mTitle);
        form.setContent(mContent);
        form.appendDivider();
        _buildTopControlButtons(form, localeCode);
        form.appendDivider();
        _buildPageButton(form, page);
        form.appendDivider();
        _buildBottomControlButtons(form, localeCode);
    }

    void sendSpecificPage(Player& player, int page) {
        if (mDirty) {
            countTotalPages();
            mDirty = false;
        }
        if (page < 1 || page > mTotalPages) {
            return;
        }
        SimpleForm stackForm;
        buildPage(stackForm, page, player.getLocaleCode());
        stackForm.sendTo(player);
        mCurrentPage = page;
    }
    void sendFirstPage(Player& player) { sendSpecificPage(player, 1); }
    void sendLastPage(Player& player) { sendSpecificPage(player, mTotalPages); }
    void sendNextPage(Player& player) {
        if (mCurrentPage + 1 > mTotalPages) {
            sendSpecificPage(player, mTotalPages); // 循环
        } else {
            sendSpecificPage(player, mCurrentPage + 1);
        }
    }
    void sendPreviousPage(Player& player) {
        if (mCurrentPage - 1 < 1) {
            sendSpecificPage(player, 1); // 循环
        } else {
            sendSpecificPage(player, mCurrentPage - 1);
        }
    }
};

PaginatedForm::Options::Options() = default;

PaginatedForm::PaginatedForm(Options options) : impl(std::make_shared<Impl>()) { impl->mOptions = std::move(options); }
PaginatedForm::~PaginatedForm() = default;

PaginatedForm& PaginatedForm::setTitle(std::string title) {
    impl->mTitle = std::move(title);
    return *this;
}

PaginatedForm& PaginatedForm::setContent(std::string content) {
    impl->mContent = std::move(content);
    return *this;
}

PaginatedForm&
PaginatedForm::appendButton(std::string text, std::string imageData, std::string imageType, ButtonCallback callback) {
    impl->mButtons.emplace_back(text, imageData, imageType, std::move(callback));
    impl->mDirty = true;
    return *this;
}

PaginatedForm& PaginatedForm::appendButton(std::string text, ButtonCallback callback) {
    impl->mButtons.emplace_back(text, "", "", std::move(callback));
    impl->mDirty = true;
    return *this;
}

void PaginatedForm::sendTo(Player& player) { impl->sendFirstPage(player); }


} // namespace gui
} // namespace land