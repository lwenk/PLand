#pragma once
#include "ll/api/Expected.h"
#include "pland/Global.h"

#include <optional>
#include <string>
#include <string_view>


namespace land {


class StringValidator {
public:
    StringValidator() = delete;

    /**
     * @brief 验证字符串
     * @param input 输入字符串
     * @param fieldName 字段名(用于错误信息)
     * @param minLen 最小长度
     * @param maxLen 最大长度
     * @param allowNewline 是否允许换行符
     * @param localeCode 本地化代码(用户异常翻译,传递给 i18n 系统)
     */
    LDNDAPI static ll::Expected<> validate(
        std::string_view           input,
        std::string_view           fieldName,
        size_t                     minLen,
        size_t                     maxLen,
        bool                       allowNewline = false,
        std::optional<std::string> localeCode   = std::nullopt
    );
};


} // namespace land