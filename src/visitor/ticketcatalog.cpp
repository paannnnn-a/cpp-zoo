#include "visitor/ticketcatalog.h"
#include "visitor/visitorutils.h"
using namespace std;
TicketCatalog::TicketCatalog() {
}
///实现票种查找，这里没看懂
Ticket* TicketCatalog::findticket(TicketType type) {
    if (type == TicketType::adult) {
        return &adult;
    }
    if (type == TicketType::student) {
        return &student;
    }
    if (type == TicketType::child) {
        return &child;
    }
    return nullptr;
}
const Ticket* TicketCatalog::findticket(TicketType type) const {
    if (type == TicketType::adult) {
        return &adult;
    }
    if (type == TicketType::student) {
        return &student;
    }
    if (type == TicketType::child) {
        return &child;
    }
    return nullptr;
}
///实现单个票种查询
bool TicketCatalog::getticketinfo(TicketType type, TicketInfo& info) {
    lasterror.clear();
    Ticket* ticket = findticket(type);
    if (ticket == nullptr) {
        lasterror = "票种不存在";
        return false;
    }
    TicketInfo result;
    result.type = ticket->gettype();
    result.name = ticket->getname();
    result.price = ticket->getprice();
    result.onsale = ticket->getonsale();
    result.qualification = ticket->needqualification();
    result.eligibility = ticket->geteligibility();
    info = result;
    return true;
}
///实现全部票种查询
vector<TicketInfo> TicketCatalog::getallticketinfo() const {
    vector<TicketInfo> result;
    TicketType types[3] = {
        TicketType::adult,
        TicketType::student,
        TicketType::child
    };
    for (int i = 0; i < 3; i++) {
        const Ticket* ticket = findticket(types[i]);
        TicketInfo info;
        info.type = ticket->gettype();
        info.name = ticket->getname();
        info.price = ticket->getprice();
        info.onsale = ticket->getonsale();
        info.qualification = ticket->needqualification();
        info.eligibility = ticket->geteligibility();
        result.push_back(info);
    }
    return result;
}
///实现管理员调价
bool TicketCatalog::updateprice(const Session& session,
    TicketType type,
    Money price) {
    lasterror.clear();
    if (session.getrole() != Role::admin) {
        lasterror = "只有管理员可以修改票价";
        return false;
    }
    Ticket* ticket = findticket(type);
    if (ticket == nullptr) {
        lasterror = "票种不存在";
        return false;
    }
    if (!ticket->setprice(price)) {
        lasterror = "票价必须大于零";
        return false;
    }
    return true;
}
///管理员控制票种上架情况
bool TicketCatalog::setonsale(const Session& session,
    TicketType type,
    bool onsale) {
    lasterror.clear();
    if (session.getrole() != Role::admin) {
        lasterror = "只有管理员可以修改售票状态";
        return false;
    }
    Ticket* ticket = findticket(type);
    if (ticket == nullptr) {
        lasterror = "票种不存在";
        return false;
    }
    ticket->setonsale(onsale);
    return true;
}
///保存票价和售票状态
bool TicketCatalog::savetofile(const string& filename) {
    lasterror.clear();
    vector<TicketInfo> infos = getallticketinfo();
    string codes[3] = { "adult", "student", "child" };
    for (int i = 0; i < 3; i++) {
        if (infos[i].price <= 0) {
            lasterror = "票价必须大于零，无法保存";
            return false;
        }
    }
    ofstream file(filename);
    if (!file.is_open()) {
        lasterror = "无法打开票种文件";
        return false;
    }
    for (int i = 0; i < 3; i++) {
        file << codes[i] << "|"
            << infos[i].price << "|"
            << infos[i].onsale << "\n";
    }
    file.close();
    if (file.fail()) {
        lasterror = "票种文件写入失败";
        return false;
    }
    return true;
}
///读取票种和售票状态
bool TicketCatalog::loadfromfile(const string& filename) {
    lasterror.clear();
    ifstream file(filename);
    if (!file.is_open()) {
        lasterror = "无法打开票种文件";
        return false;
    }
    Money prices[3] = { 0, 0, 0 };
    bool states[3] = { false, false, false };
    bool found[3] = { false, false, false };
    string line;
    int linenumber = 0;
    while (getline(file, line)) {
        linenumber++;
        if (linenumber == 1 && line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }
        vector<string> fields;
        string field;
        stringstream row(line);
        while (getline(row, field, '|')) {
            fields.push_back(field);
        }
        if (fields.size() != 3 || line.back() == '|') {
            lasterror = "第" + to_string(linenumber) + "行字段数量错误";
            return false;
        }
        int index = -1;
        if (fields[0] == "adult") {
            index = 0;
        }
        else if (fields[0] == "student") {
            index = 1;
        }
        else if (fields[0] == "child") {
            index = 2;
        }
        else {
            lasterror = "第" + to_string(linenumber) + "行票种不存在";
            return false;
        }
        if (found[index]) {
            lasterror = "第" + to_string(linenumber) + "行票种重复";
            return false;
        }
        if (fields[1].empty()) {
            lasterror = "第" + to_string(linenumber) + "行价格为空";
            return false;
        }
        for (size_t i = 0; i < fields[1].size(); i++) {
            if (fields[1][i] < '0' || fields[1][i] > '9') {
                lasterror = "第" + to_string(linenumber) + "行价格必须为整数分";
                return false;
            }
        }
        Money price = 0;
        stringstream number(fields[1]);
        if (!(number >> price) || !number.eof() || price <= 0) {
            lasterror = "第" + to_string(linenumber) + "行价格超出范围或不大于零";
            return false;
        }
        if (fields[2] != "0" && fields[2] != "1") {
            lasterror = "第" + to_string(linenumber) + "行售票状态只能为0或1";
            return false;
        }
        prices[index] = price;
        states[index] = fields[2] == "1";
        found[index] = true;
    }
    if (file.bad() || !file.eof()) {
        lasterror = "读取票种文件失败";
        return false;
    }
    for (int i = 0; i < 3; i++) {
        if (!found[i]) {
            lasterror = "票种文件必须包含成人票、学生票和儿童票";
            return false;
        }
    }
    // 将完整配置写入三个票种对象
    TicketType types[3] = {
        TicketType::adult,
        TicketType::student,
        TicketType::child
    };
    for (int i = 0; i < 3; i++) {
        Ticket* ticket = findticket(types[i]);
        ticket->setprice(prices[i]);
        ticket->setonsale(states[i]);
    }
    return true;
}
string TicketCatalog::getlasterror() const {
    return lasterror;
}
///节日优惠
int TicketCatalog::findholiday(int year, int month, int day) const {
    for (size_t i = 0; i < holidays.size(); i++) {
        if (holidays[i].year == year
            && holidays[i].month == month
            && holidays[i].day == day) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
bool TicketCatalog::validholiday(int year, int month, int day) const {
    if (year < 0 || year > 9999 || month < 1 || month > 12) {
        return false;
    }
    int days[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    if (month == 2) {
        // 每年规则允许设置2月29日
        if (year == 0 || year % 400 == 0
            || (year % 4 == 0 && year % 100 != 0)) {
            days[1] = 29;
        }
    }
    return day >= 1 && day <= days[month - 1];
}
bool TicketCatalog::addholiday(const Session& session,
    int year, int month, int day) {
    lasterror.clear();
    if (session.getrole() != Role::admin) {
        lasterror = "只有管理员可以添加节日规则";
        return false;
    }
    if (!validholiday(year, month, day)) {
        lasterror = "节日日期不合法，年份应为0至9999，月日必须有效";
        return false;
    }
    if (findholiday(year, month, day) != -1) {
        lasterror = "该节日规则已经存在";
        return false;
    }
    HolidayRule rule;
    rule.year = year;
    rule.month = month;
    rule.day = day;
    holidays.push_back(rule);
    return true;
}
bool TicketCatalog::removeholiday(const Session& session,
    int year, int month, int day) {
    lasterror.clear();
    if (session.getrole() != Role::admin) {
        lasterror = "只有管理员可以删除节日规则";
        return false;
    }
    if (!validholiday(year, month, day)) {
        lasterror = "节日日期不合法，年份应为0至9999，月日必须有效";
        return false;
    }
    int index = findholiday(year, month, day);
    if (index == -1) {
        lasterror = "该节日规则不存在";
        return false;
    }
    holidays.erase(holidays.begin() + index);
    return true;
}
bool TicketCatalog::getholidays(const Session& session,
    vector<HolidayRule>& result) {
    lasterror.clear();
    if (session.getrole() != Role::admin) {
        lasterror = "只有管理员可以查询节日规则";
        return false;
    }
    result = holidays;
    return true;
}
bool TicketCatalog::isholiday(const string& date) const {
    if (!DateUtils::validdate(date)) {
        return false;
    }
    int year = stoi(date.substr(0, 4));
    int month = stoi(date.substr(5, 2));
    int day = stoi(date.substr(8, 2));
    for (size_t i = 0; i < holidays.size(); i++) {
        const HolidayRule& rule = holidays[i];
        if (rule.month == month && rule.day == day
            && (rule.year == 0 || rule.year == year)) {
            return true;
        }
    }
    return false;
}
bool TicketCatalog::saveholidaystofile(const string& filename) {
    lasterror.clear();
    for (size_t i = 0; i < holidays.size(); i++) {
        const HolidayRule& rule = holidays[i];
        if (!validholiday(rule.year, rule.month, rule.day)) {
            lasterror = "节日规则包含不合法的日期，无法保存";
            return false;
        }
    }
    ofstream file(filename);
    if (!file.is_open()) {
        lasterror = "无法打开节日规则文件";
        return false;
    }
    for (size_t i = 0; i < holidays.size(); i++) {
        const HolidayRule& rule = holidays[i];
        file << rule.year << "|"
            << rule.month << "|"
            << rule.day << "\n";
    }
    file.close();
    if (file.fail()) {
        lasterror = "节日规则文件写入失败";
        return false;
    }
    return true;
}
bool TicketCatalog::loadholidaysfromfile(const string& filename) {
    lasterror.clear();
    ifstream file(filename);
    if (!file.is_open()) {
        lasterror = "无法打开节日规则文件";
        return false;
    }
    vector<HolidayRule> loaded;
    string line;
    int linenumber = 0;
    while (getline(file, line)) {
        linenumber++;
        if (linenumber == 1
            && line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }
        stringstream row(line);
        HolidayRule rule;
        char first;
        char second;
        char extra;
        if (!(row >> rule.year >> first >> rule.month
            >> second >> rule.day)
            || first != '|' || second != '|' || (row >> extra)) {
            lasterror = "节日文件第" + to_string(linenumber)
                + "行格式错误，应为年份|月份|日期";
            return false;
        }
        if (!validholiday(rule.year, rule.month, rule.day)) {
            lasterror = "节日文件第" + to_string(linenumber)
                + "行日期不合法";
            return false;
        }
        for (size_t i = 0; i < loaded.size(); i++) {
            if (loaded[i].year == rule.year
                && loaded[i].month == rule.month
                && loaded[i].day == rule.day) {
                lasterror = "节日文件第" + to_string(linenumber)
                    + "行规则重复";
                return false;
            }
        }
        loaded.push_back(rule);
    }
    if (file.bad() || !file.eof()) {
        lasterror = "读取节日规则文件失败";
        return false;
    }
    holidays.swap(loaded);
    return true;
}