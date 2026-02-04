#pragma once
#include <string>
#include <string_view>


namespace land::string_utils {

// Minecraft 颜色符号
inline constexpr std::string_view COLOR_STR = "§";

/**
 * @brief 检查字符串是否为有效的 UTF-8 序列
 */
inline bool isValidUtf8(std::string_view sv) {
    auto iter = sv.begin();
    auto end  = sv.end();
    while (iter != end) {
        unsigned char c = static_cast<unsigned char>(*iter);
        if (c < 0x80) { // ASCII (1 byte)
            iter++;
        } else if ((c & 0xE0) == 0xC0) { // 2 bytes
            if (std::distance(iter, end) < 2) return false;
            if ((static_cast<unsigned char>(*(iter + 1)) & 0xC0) != 0x80) return false;
            iter += 2;
        } else if ((c & 0xF0) == 0xE0) { // 3 bytes
            if (std::distance(iter, end) < 3) return false;
            if ((static_cast<unsigned char>(*(iter + 1)) & 0xC0) != 0x80
                || (static_cast<unsigned char>(*(iter + 2)) & 0xC0) != 0x80)
                return false;
            iter += 3;
        } else if ((c & 0xF8) == 0xF0) { // 4 bytes
            if (std::distance(iter, end) < 4) return false;
            if ((static_cast<unsigned char>(*(iter + 1)) & 0xC0) != 0x80
                || (static_cast<unsigned char>(*(iter + 2)) & 0xC0) != 0x80
                || (static_cast<unsigned char>(*(iter + 3)) & 0xC0) != 0x80)
                return false;
            iter += 4;
        } else {
            return false; // Invalid start byte
        }
    }
    return true;
}

/**
 * @brief 获取 UTF-8 字符串的字符数量 (Code Points)
 */
inline size_t getUtf8Length(std::string_view sv) {
    size_t length = 0;
    for (size_t i = 0; i < sv.size();) {
        unsigned char c = static_cast<unsigned char>(sv[i]);

        // 如果是多字节开头，跳过对应长度；
        // 如果是 ASCII (0xxxxxxx) 或者 非法字节 (10xxxxxx, 11111xxx等无法匹配的情况)，都跳过 1 字节
        if ((c & 0xE0) == 0xC0) {
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            i += 4;
        } else {
            i += 1; // Handles ASCII and Invalid bytes
        }
        length++;
    }
    return length;
}

/**
 * @brief 检查是否包含非法控制字符
 * @param allowNewline 是否允许换行符
 * @return true 如果包含非法字符
 */
inline bool hasIllegalControlChars(std::string_view sv, bool allowNewline = false) {
    for (char c : sv) {
        unsigned char uc = static_cast<unsigned char>(c);
        // ASCII 控制字符范围 0x00-0x1F (0-31) 以及 0x7F (DEL)
        if (uc < 32 || uc == 127) {
            // 如果允许换行，且当前字符是 \n (10) 或 \r (13)，则跳过（合法）
            if (allowNewline && (uc == '\n' || uc == '\r')) {
                continue;
            }
            return true;
        }
    }
    return false;
}


} // namespace land::string_utils