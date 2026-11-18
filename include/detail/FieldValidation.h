#ifndef ZOO_DETAIL_FIELDVALIDATION_H
#define ZOO_DETAIL_FIELDVALIDATION_H

#include "../AnimalCommon.h"
#include <cctype>
#include <stdexcept>
#include <string>

namespace zoo_detail {

/**
 * @brief 检查文本是否符合竖线分隔文件的字段规则。
 * @param value 待检查文本，不修改原文。
 * @param required true 时拒绝空串和纯空白；false 时允许描述留空。
 * @return 不含分隔符、回车、换行且满足非空要求时返回 true。
 */
inline bool validText(const std::string& value, bool required = true) {
    bool hasContent = false;
    for (unsigned char ch : value) {
        if (ch == '|' || ch == '\r' || ch == '\n') return false;
        // 使用 unsigned char，避免中文编码的负 char 值传给 isspace。
        if (!std::isspace(ch)) hasContent = true;
    }
    return !required || hasContent;
}

/**
 * @brief 检查编号非空，且不含空白和文件字段分隔符。
 * @param value 待检查编号；不额外限制编号前缀和长度。
 * @return 所有字符均满足规则时返回 true。
 */
inline bool validId(const std::string& value) {
    if (value.empty()) return false;
    for (unsigned char ch : value) {
        if (ch == '|' || std::isspace(ch)) return false;
    }
    return true;
}

/**
 * @brief 校验必填编号，失败时抛出带字段名的异常。
 * @param value 待检查编号。
 * @param field 用于错误提示的字段名。
 * @throws std::invalid_argument 编号不符合 validId 规则。
 */
inline void requireId(const std::string& value, const std::string& field) {
    if (!validId(value))
        throw std::invalid_argument(field + ": ID must be nonempty and contain no whitespace or '|'");
}

/**
 * @brief 复用文本规则，在赋值前拒绝非法字段。
 * @param value 待检查文本。
 * @param field 用于错误提示的字段名。
 * @param required 是否要求包含非空白内容，默认 true。
 * @throws std::invalid_argument 文本不符合 validText 规则。
 */
inline void requireText(const std::string& value, const std::string& field,
                        bool required = true) {
    if (!validText(value, required))
        throw std::invalid_argument(field + ": invalid text (empty required field or '|', CR, LF)");
}

/**
 * @brief 显式枚举合法状态，拒绝强制转换产生的越界值。
 * @param status 待检查的动物状态。
 * @return 属于 AnimalStatus 定义的五种状态时返回 true。
 */
inline bool validStatus(AnimalStatus status) {
    switch (status) {
    case AnimalStatus::NORMAL:
    case AnimalStatus::TREATMENT:
    case AnimalStatus::ISOLATED:
    case AnimalStatus::TRANSFERRED:
    case AnimalStatus::DECEASED:
        return true;
    }
    return false;
}

} // namespace zoo_detail

#endif
