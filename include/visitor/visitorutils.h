#pragma once
#include <string>
using namespace std;
/// 检查文本及编号格式
class FieldValidation {
public:
    /// 检查文本非空白且不包含竖线或换行
    static bool validtext(const string& text);
    /// 检查编号非空且不包含空白或竖线
    static bool validid(const string& id);
};

/// 检查日期并获取本地时间
class DateUtils {
public:
    /// 检查 YYYY-MM-DD 格式及日期是否真实存在
    static bool validdate(const string& date);
    /// 检查 YYYY-MM-DD HH:MM:SS 格式及日期时间是否合法
    static bool validdatetime(const string& datetime);
    /// 获取今天的日期
    static string today();
    /// 获取当前时间
    static string now();
    /// 计算日期经过指定天数后的结果
    static string adddays(const string& date, int days);
};
