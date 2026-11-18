#ifndef ZOO_DETAIL_TEXTIO_H
#define ZOO_DETAIL_TEXTIO_H

#include "FieldValidation.h"
#include <charconv>
#include <fstream>
#include <vector>
#include <system_error>

namespace zoo_detail {
// 严格解析非负整数；失败不改变输出值，拒绝溢出和尾随字符。
inline bool integer(const std::string& text, int& out) {
    if (text.empty() || text.front() == '-') return false;
    int value = 0;
    const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
    if (result.ec != std::errc{} || result.ptr != text.data() + text.size() || value < 0)
        return false;
    out = value;
    return true;
}

// 检查 YYYY-MM-DD 的真实公历日期，包括世纪闰年规则。
inline bool date(const std::string& text) {
    if (text.size() != 10 || text[4] != '-' || text[7] != '-') return false;
    for (std::size_t i = 0; i < text.size(); ++i)
        if (i != 4 && i != 7 && (text[i] < '0' || text[i] > '9')) return false;
    int year, month, day;
    if (!integer(text.substr(0, 4), year) || !integer(text.substr(5, 2), month) ||
        !integer(text.substr(8, 2), day) || year < 1 || month < 1 || month > 12) return false;
    const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const bool leap = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
    return day >= 1 && day <= days[month - 1] + (month == 2 && leap ? 1 : 0);
}

/**
 * @brief 逐行读取竖线分隔文本，保留末尾空字段，兼容 BOM 和 CRLF。
 * @param file 输入文件路径，文件不存在时报告失败。
 * @param error 输出错误信息；成功时为空。
 * @param consume 接收字段数组的回调，返回 false 或抛出字段异常时停止。
 * @note 回调应写入临时容器，由调用方在全部成功后提交。
 */
template<class Consumer>
bool readRows(const std::string& file, std::string& error, Consumer consume) {
    error.clear();
    std::ifstream input(file, std::ios::binary);
    if (!input) { error = "Cannot open file: " + file; return false; }
    std::string line;
    std::size_t number = 0;
    while (std::getline(input, line)) {
        ++number;
        if (number == 1 && line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::vector<std::string> fields;
        std::size_t begin = 0;
        for (;;) {
            const auto end = line.find('|', begin);
            fields.push_back(line.substr(begin, end == std::string::npos ? end : end - begin));
            if (end == std::string::npos) break;
            begin = end + 1;
        }
        try {
            if (consume(fields)) continue;
            error = "Invalid fields";
        } catch (const std::invalid_argument& e) { error = e.what(); }
        error = file + ": line " + std::to_string(number) + ": " + error;
        return false;
    }
    if (input.bad() || (input.fail() && !input.eof())) {
        error = "Read failed: " + file;
        return false;
    }
    return true;
}

// 覆盖写入文本，并检查关闭后的状态以发现延迟写入错误。
inline bool writeText(const std::string& file, const std::string& text, std::string& error) {
    error.clear();
    std::ofstream output(file, std::ios::binary | std::ios::trunc);
    if (!output) { error = "Cannot write file: " + file; return false; }
    output << text;
    output.close();
    if (!output) { error = "Write failed: " + file; return false; }
    return true;
}
} // namespace zoo_detail
#endif
