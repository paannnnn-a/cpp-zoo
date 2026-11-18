#include "visitor/visitormenu.h"
#include "visitor/visitorutils.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <climits>
using namespace std;

///读取文字和整数
VisitorMenu::VisitorMenu(const ZoneManager& zonemanager,
    const AnimalManager& animalmanager,
    const string& datadirectory)
    : zones(zonemanager),
    animals(animalmanager),
    coupons(visitors),
    tickets(visitors, catalog, coupons),
    feedback(visitors, tickets),
    auth(visitors),
    routes(zones, animals),
    storage(datadirectory, visitors, catalog, coupons, tickets, feedback),
    logger(datadirectory + "/operation_logs.txt") {
}
bool VisitorMenu::load() {
    lasterror.clear();
    if (!storage.load()) {
        lasterror = storage.getlasterror();
        return false;
    }
    return true;
}
bool VisitorMenu::save() {
    lasterror.clear();
    if (!storage.save()) {
        lasterror = storage.getlasterror();
        return false;
    }
    return true;
}
string VisitorMenu::getlasterror() const {
    return lasterror;
}
bool VisitorMenu::readtext(const string& prompt,
    string& text,
    bool allowempty) {
    cout << prompt;
    if (allowempty) {
        cout << "（可直接回车留空）：";
    }
    else {
        cout << "（直接回车取消）：";
    }
    string input;
    if (!getline(cin, input)) {
        return false;
    }
    if (input.empty() && !allowempty) {
        cout << "已取消本次操作\n";
        return false;
    }
    text = input;
    return true;
}
bool VisitorMenu::readint(const string& prompt,
    int minimum,
    int maximum,
    int& value) {
    while (true) {
        string input;
        if (!readtext(prompt, input)) {
            return false;
        }
        stringstream number(input);
        int result = 0;
        char extra;
        if (!(number >> result) || (number >> extra)) {
            cout << "请输入范围内的整数\n";
            continue;
        }
        if (result < minimum || result > maximum) {
            cout << "请输入" << minimum << "至" << maximum
                << "之间的整数\n";
            continue;
        }
        value = result;
        return true;
    }
}
///金额的输入和显示
bool VisitorMenu::readmoney(const string& prompt, Money& value) {
    while (true) {
        string input;
        if (!readtext(prompt + "，单位为元，最多两位小数", input)) {
            return false;
        }
        string digits = input;
        int decimals = 0;
        size_t point = input.find('.');
        if (point != string::npos) {
            if (point == 0 || input.size() - point - 1 < 1
                || input.size() - point - 1 > 2) {
                cout << "金额格式错误，例如50或50.25\n";
                continue;
            }
            decimals = static_cast<int>(input.size() - point - 1);
            digits.erase(point, 1);
        }
        bool valid = true;
        for (size_t i = 0; i < digits.size(); i++) {
            if (digits[i] < '0' || digits[i] > '9') {
                valid = false;
                break;
            }
        }
        if (!valid) {
            cout << "金额只能包含数字和一个小数点\n";
            continue;
        }
        stringstream number(digits);
        Money result = 0;
        if (!(number >> result) || !number.eof()) {
            cout << "金额超出范围\n";
            continue;
        }
        // 将元转换为分
        for (int i = decimals; i < 2; i++) {
            if (result > LLONG_MAX / 10) {
                valid = false;
                break;
            }
            result *= 10;
        }
        if (!valid) {
            cout << "金额超出范围\n";
            continue;
        }
        value = result;
        return true;
    }
}
string VisitorMenu::moneytext(Money amount) const {
    Money yuan = amount / 100;
    int fen = static_cast<int>(amount % 100);
    ostringstream result;
    if (amount < 0) {
        result << "-";
        yuan = -yuan;
        fen = -fen;
    }
    result << yuan << "." << setfill('0') << setw(2) << fen;
    return result.str();
}
///操作确认
bool VisitorMenu::readticketids(const string& prompt,
    vector<string>& ids) {
    while (true) {
        string input;
        if (!readtext(prompt + "，多个票号用空格分隔", input)) {
            return false;
        }
        stringstream text(input);
        vector<string> result;
        string id;
        bool valid = true;
        while (text >> id) {
            if (!FieldValidation::validid(id)) {
                valid = false;
                break;
            }
            result.push_back(id);
            if (result.size() > 10) {
                valid = false;
                break;
            }
        }
        if (!valid || result.empty()) {
            cout << "请输入1至10个格式正确的电子票编号\n";
            continue;
        }
        ids = result;
        return true;
    }
}
bool VisitorMenu::confirm(const string& message) {
    while (true) {
        string answer;
        if (!readtext(message + "，输入Y确认、N取消", answer)) {
            return false;
        }
        if (answer == "Y" || answer == "y") {
            return true;
        }
        if (answer == "N" || answer == "n") {
            cout << "已取消本次操作\n";
            return false;
        }
        cout << "请输入Y或N\n";
    }
}
///菜单文字显示
string VisitorMenu::tickettypename(TicketType type) const {
    if (type == TicketType::adult) {
        return "成人票";
    }
    if (type == TicketType::student) {
        return "学生票";
    }
    if (type == TicketType::child) {
        return "儿童票";
    }
    return "未知票种";
}
string VisitorMenu::ticketstatusname(TicketStatus status) const {
    if (status == TicketStatus::unused) {
        return "未使用";
    }
    if (status == TicketStatus::used) {
        return "已入园";
    }
    if (status == TicketStatus::refunded) {
        return "已退款";
    }
    return "未知状态";
}
string VisitorMenu::feedbacktypename(FeedbackType type) const {
    if (type == FeedbackType::suggestion) {
        return "意见建议";
    }
    if (type == FeedbackType::review) {
        return "游后评价";
    }
    return "未知种类";
}
string VisitorMenu::feedbackstatusname(FeedbackStatus status) const {
    if (status == FeedbackStatus::pending) {
        return "未处理";
    }
    if (status == FeedbackStatus::processing) {
        return "处理中";
    }
    if (status == FeedbackStatus::resolved) {
        return "已处理";
    }
    return "未知状态";
}
///统一显示操作结果
void VisitorMenu::finishoperation(const Session& actor,
    const string& action,
    const string& targetid,
    bool success,
    const string& error) {
    string description;
    if (!success) {
        description = error;
        if (description.empty()) {
            description = "操作失败";
        }
        cout << description << "\n";
    }
    else {
        if (save()) {
            description = "操作成功，数据已保存";
            cout << description << "\n";
        }
        else {
            description = "操作已完成，但数据保存失败：" + lasterror;
            cout << description << "\n";
            cout << "当前修改仍保留在内存中，请重试保存\n";
        }
        if (!targetid.empty()) {
            cout << "相关编号：" << targetid << "\n";
        }
    }
    if (!logger.writelog(actor, action, targetid, success, description)) {
        cout << "日志写入失败：" << logger.getlasterror() << "\n";
    }
}
///菜单主入口
char VisitorMenu::run() {
    while (true) {
        cout << "\n========== 游客与票务服务 ==========\n";
        cout << "当前用户：" << session.getdisplayname() << "\n";
        if (session.getrole() == Role::guest) {
            cout << "当前身份：未登录\n";
            cout << "1. 注册账号\n";
            cout << "2. 登录\n";
            cout << "3. 查询动物\n";
            cout << "4. 查看推荐路线\n";
        }
        else if (session.getrole() == Role::visitor) {
            cout << "当前身份：游客\n";
            cout << "1. 个人账号\n";
            cout << "2. 票务服务\n";
            cout << "3. 游园查询与反馈\n";
            cout << "L. 退出登录\n";
        }
        else {
            cout << "当前身份：票务管理员\n";
            cout << "1. 管理员功能\n";
            cout << "2. 查询动物\n";
            cout << "3. 查看推荐路线\n";
            cout << "L. 退出登录\n";
        }
        cout << "S. 保存数据\n";
        cout << "0/M. 返回系统主菜单\n";
        cout << "Q. 请求退出系统\n";
        cout << "请选择：";
        string choice;
        if (!getline(cin, choice)) {
            if (!save()) {
                cout << "保存失败：" << lasterror << "\n";
            }
            return 'Q';
        }
        char navigation = '0';
        if (choice == "0" || choice == "M" || choice == "m") {
            navigation = 'M';
        }
        else if (choice == "Q" || choice == "q") {
            navigation = 'Q';
        }
        else if (choice == "S" || choice == "s") {
            if (save()) {
                cout << "数据已保存\n";
            }
            else {
                cout << "保存失败：" << lasterror << "\n";
            }
        }
        else if (session.getrole() == Role::guest) {
            if (choice == "1") {
                registervisitor();
            }
            else if (choice == "2") {
                login();
            }
            else if (choice == "3") {
                browseanimals();
            }
            else if (choice == "4") {
                showroutes();
            }
            else {
                cout << "无效选项，请重新输入\n";
            }
        }
        else if (choice == "L" || choice == "l") {
            logout();
        }
        else if (session.getrole() == Role::visitor) {
            if (choice == "1") {
                navigation = accountmenu();
            }
            else if (choice == "2") {
                navigation = ticketmenu();
            }
            else if (choice == "3") {
                navigation = servicemenu();
            }
            else {
                cout << "无效选项，请重新输入\n";
            }
        }
        else {
            if (choice == "1") {
                navigation = adminmenu();
            }
            else if (choice == "2") {
                browseanimals();
            }
            else if (choice == "3") {
                showroutes();
            }
            else {
                cout << "无效选项，请重新输入\n";
            }
        }
        if (navigation == 'M' || navigation == 'Q') {
            if (save()) {
                return navigation;
            }
            cout << "保存失败：" << lasterror << "\n";
            if (!cin.good()) {
                return 'Q';
            }
            cout << "请处理问题后选择S重试保存\n";
        }
    }
}