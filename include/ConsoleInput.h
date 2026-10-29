#pragma once
#include "MedicalRecord.h"
#include "detail/TextIO.h"
#include <iostream>
#include <string>
namespace zoo_ui {
/**
 * @brief 在输入层与菜单层之间携带取消或导航请求。
 * @note code为'0'/'M'/'Q'；通过抛出和捕获该值中断本次尚未提交的录入。
 */
struct Navigate {
    char code;
};
/**
 * @brief 逐行读取合法文本，并将取消/主菜单/退出转换为导航请求。
 * @param label 界面输入提示文字。
 * @param emptyAllowed 是否允许空或纯空白文本，默认false。
 * @return 合法输入原文，不自动去除空格。
 * @note 遇0/M/Q或EOF抛出Navigate；非法文本会提示重输。
 */
inline std::string ask(const std::string &label, bool emptyAllowed = false) {
    for (;;) {
        std::cout << label << " [0取消/M主菜单/Q退出]：";
        std::string s;
        if (!std::getline(std::cin, s))
            throw Navigate{'Q'};
        if (s == "0")
            throw Navigate{'0'};
        if (s == "M" || s == "m")
            throw Navigate{'M'};
        if (s == "Q" || s == "q")
            throw Navigate{'Q'};
        if (zoo_detail::validText(s, !emptyAllowed))
            return s;
        std::cout << "输入不能为空或包含 |、换行。\n";
    }
}
/**
 * @brief 循环读取非负整数，拒绝非法字符、负数和int溢出。
 * @param label 数字输入项提示文字。
 * @return 合法的非负int。
 * @note 输入0取消；合法数值0请填00；导航请求继续向上传播。
 */
inline int number(const std::string &label) {
    for (;;) {
        auto s = ask(label + "（数字0请填00）");
        int n = 0;
        if (zoo_detail::integer(s, n))
            return n;
        std::cout << "请输入非负整数，不能溢出 int。\n";
    }
}
/**
 * @brief 要求Y确认或N取消，非法答案会重新提示。
 * @param text 待确认操作的具体描述，建议包含动物编号和变更前后状态。
 * @return void；用户输入Y时正常返回。
 * @note N/0抛出Navigate{0}；M/Q/EOF同样传播导航请求。
 */
inline void confirm(const std::string &text) {
    for (;;) {
        auto s = ask(text + "，Y确认/N取消");
        if (s == "Y" || s == "y")
            return;
        if (s == "N" || s == "n")
            throw Navigate{'0'};
        std::cout << "请输入Y或N。\n";
    }
}
/**
 * @brief 输出一条动物公共基本信息和展出说明。
 * @param i 只读动物信息副本。
 * @return void；输出到std::cout。
 */
inline void show(const AnimalBasicInfo &i) {
    std::cout << i.animalId << " | " << i.name << " | " << i.category << " | " << i.species << " | "
              << i.zoneName << " | " << animalStatusToString(i.status) << " | "
              << (i.onDisplay ? "正常展出" : "暂不展出") << '\n';
}
/**
 * @brief 逐条显示查询结果，空列表显示没有匹配档案。
 * @param rows 动物基本信息副本列表。
 * @return void。
 */
inline void list(const std::vector<AnimalBasicInfo> &rows) {
    if (rows.empty())
        std::cout << "没有匹配档案。\n";
    for (const auto &r : rows)
        show(r);
}
/**
 * @brief 显示医疗记录列表，空列表显示暂无医疗记录。
 * @param rows 记录副本列表；顺序由HealthManager查询结果决定。
 * @return void。
 */
inline void medical(const std::vector<MedicalRecordInfo> &rows) {
    if (rows.empty())
        std::cout << "暂无医疗记录。\n";
    for (const auto &r : rows)
        std::cout << r.recordId << " | " << r.date << " | " << r.veterinarianId << " | " << r.type
                  << " | " << r.description << '\n';
}
} // namespace zoo_ui
