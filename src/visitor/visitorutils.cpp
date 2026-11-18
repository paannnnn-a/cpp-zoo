#include "visitor/visitorutils.h"
#include <ctime>
#include <iomanip>
#include <sstream>
using namespace std;
/// 判断是否为闰年
static bool leapyear(int year) {
    return year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
}
/// 获取指定月份的天数
static int monthdays(int year, int month) {
    int days[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (month == 2 && leapyear(year)) {
        return 29;
    }
    return days[month - 1];
}
/// 将日期转换为固定格式字符串
static string formatdate(int year, int month, int day) {
    ostringstream result;
    result << setfill('0') << setw(4) << year << "-"
        << setw(2) << month << "-" << setw(2) << day;
    return result.str();
}
/// 将日期转换成序号，以方便计算
static int dateordinal(int year, int month, int day) {
    int previousyear = year - 1;
    int result = previousyear * 365 + previousyear / 4
        - previousyear / 100 + previousyear / 400;
    for (int i = 1; i < month; i++) {
        result += monthdays(year, i);
    }
    return result + day;
}
///文本校验
/// 判断是否为空白符
static bool isblankchar(char c) {
    return (c == ' ' || c == '\t' || c == '\n'
        || c == '\r' || c == '\v' || c == '\f');
}
///检查字符
bool FieldValidation::validtext(const string& text) {
    bool hascontent = false;
    for (size_t i = 0; i < text.size(); i++) {
        char c = text[i];
        if (c == '|' || c == '\r' || c == '\n' || c == '\0') {
            return false;
        }
        if (!isblankchar(c)) {
            hascontent = true;
        }
    }
    return hascontent;
}
///检查游客编号和订单编号格式
bool FieldValidation::validid(const string& id) {
    if (id.empty()) {
        return false;
    }
    for (size_t i = 0; i < id.size(); i++) {
        char c = id[i];
        if (isblankchar(c) || c == '|' || c == '\0') {
            return false;
        }
    }
    return true;
}
bool DateUtils::validdate(const string& date) {
    ///检查格式
    if (date.size() != 10) {
        return false;
    }
    if (date[4] != '-' || date[7] != '-') {
        return false;
    }
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            continue;
        }
        if (date[i] < '0' || date[i] > '9') {
            return false;
        }
    }
    ///检查日期
    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));
    if (year < 1 || month < 1 || month > 12) {
        return false;
    }
    return day >= 1 && day <= monthdays(year, month);
}
///检查时刻
bool DateUtils::validdatetime(const string& datetime) {
    if (datetime.size() != 19) {
        return false;
    }
    if (!validdate(datetime.substr(0, 10))) {
        return false;
    }
    if (datetime[10] != ' ' || datetime[13] != ':' || datetime[16] != ':') {
        return false;
    }
    for (int i = 11; i < 19; i++) {
        if (i == 13 || i == 16) {
            continue;
        }
        if (datetime[i] < '0' || datetime[i] > '9') {
            return false;
        }
    }
    int hour = stoi(datetime.substr(11, 2));
    int minute = stoi(datetime.substr(14, 2));
    int second = stoi(datetime.substr(17, 2));
    return hour < 24 && minute < 60 && second < 60;
}
string DateUtils::now() {
    time_t current = time(nullptr);
    if (current == static_cast<time_t>(-1)) {
        return "";
    }
    tm local = {};
    if (localtime_s(&local, &current) != 0) {
        return "";
    }
    char buffer[20];
    if (strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &local) == 0) {
        return "";
    }
    string result = buffer;
    if (!validdatetime(result)) {
        return "";
    }
    return result;
}
string DateUtils::today() {
    string current = now();
    if (current.empty()) {
        return "";
    }
    return current.substr(0, 10);///截取日期
}
string DateUtils::adddays(const string& date, int days) {
    if (!validdate(date)) {
        return "";
    }
    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));
    long long target = dateordinal(year, month, day);
    target += days;
    if (target < 1 || target > dateordinal(9999, 12, 31)) {
        return "";///我们设定日期数据最多到9999年年底
    }
    ///计算年份
    year = 1;
    int yearlength = leapyear(year) ? 366 : 365;
    while (target > yearlength) {
        target -= yearlength;
        year++;
        yearlength = leapyear(year) ? 366 : 365;
    }
    ///计算月份
    month = 1;
    while (target > monthdays(year, month)) {
        target -= monthdays(year, month);
        month++;
    }
    day = static_cast<int>(target);
    return formatdate(year, month, day);
}