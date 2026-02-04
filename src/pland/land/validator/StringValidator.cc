#include "StringValidator.h"

#include "pland/utils/StringUtils.h"

#include "ll/api/Expected.h"
#include "ll/api/i18n/I18n.h"
#include <optional>

namespace land {

using ll::operator""_trl;

ll::Expected<> StringValidator::validate(
    std::string_view           input,
    std::string_view           fieldName,
    size_t                     minLen,
    size_t                     maxLen,
    bool                       allowNewline,
    std::optional<std::string> localeCode
) {
    auto locale = localeCode.value_or("zh_CN");
    if (input.empty()) {
        if (minLen > 0) return ll::makeStringError("{}不能为空"_trl(locale, fieldName));
        return {};
    }

    if (!string_utils::isValidUtf8(input)) {
        return ll::makeStringError("{}包含无效字符"_trl(locale, fieldName));
    }

    size_t charCount = string_utils::getUtf8Length(input);
    if (charCount < minLen) {
        return ll::makeStringError("{}长度过短（最少 {} 个字符）"_trl(locale, fieldName, minLen));
    }
    if (charCount > maxLen) {
        return ll::makeStringError("{}长度过长（最多 {} 个字符）"_trl(locale, fieldName, maxLen));
    }

    if (string_utils::hasIllegalControlChars(input, allowNewline)) {
        return ll::makeStringError("{}包含非法控制字符"_trl(locale, fieldName));
    }

    return {};
}


} // namespace land