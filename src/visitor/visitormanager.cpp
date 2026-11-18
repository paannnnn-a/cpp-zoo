#include "visitor/ticketmanager.h"
#include "visitor/visitormanager.h"
#include "visitor/visitorutils.h"
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;
VisitorManager::VisitorManager() {
}
int VisitorManager::findvisitor(const string& visitorid) const {
    for (size_t i = 0; i 
        < visitors.size(); i++) {
        if (visitors[i].getvisitorid() == visitorid) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
int VisitorManager::findusername(const string& username) const {
    for (size_t i = 0; i < visitors.size(); i++) {
        if (visitors[i].getusername() == username) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
///生成编号
string VisitorManager::generateid() const {
    for (int number = 1; number <= 99999999; number++) {
        ostringstream result;///防止先把内容显示到屏幕
        result << "V" << setfill('0') << setw(4) << number;
        string id = result.str();
        if (findvisitor(id) == -1) {
            return id;
        }
    }
    return "";
}
///账号设置
bool VisitorManager::registervisitor(const string& username,
    const string& password,
    const string& nickname,
    string& visitorid) {
    lasterror.clear();
    if (!FieldValidation::validid(username)) {
        lasterror = "用户名不能为空，不能包含空白或竖线";
        return false;
    }
    if (username == "admin") {
        lasterror = "该用户名为管理员保留";
        return false;
    }
    if (findusername(username) != -1) {
        lasterror = "用户名已存在";
        return false;
    }
    if (password.size() < 6 || password.size() > 32) {
        lasterror = "密码长度必须为6至32个字节";
        return false;
    }
    if (!FieldValidation::validid(password)) {
        lasterror = "密码不能包含空白或竖线";
        return false;
    }
    if (!FieldValidation::validtext(nickname)) {
        lasterror = "昵称不能为空白，不能包含竖线或换行";
        return false;
    }
    string id = generateid();
    if (id.empty()) {
        lasterror = "无法生成游客编号";
        return false;
    }
    Visitor visitor(id, username, password, nickname, false, true);
    visitors.push_back(visitor);
    visitorid = id;
    return true;
}
///使用用户名找到游客，再确认密码和账号状态
bool VisitorManager::authenticate(const string& username,
    const string& password,
    Visitor& visitor) {
    lasterror.clear();
    int index = findusername(username);
    if (index == -1) {
        lasterror = "用户名或密码错误";
        return false;
    }
    if (!visitors[index].getactive()) {
        lasterror = "账号已注销";
        return false;
    }
    if (visitors[index].getpassword() != password) {
        lasterror = "用户名或密码错误";
        return false;
    }
    visitor = visitors[index];
    return true;
}
///游客只能查自己的账号
bool VisitorManager::getvisitorinfo(const Session& session,
    const string& visitorid,
    Visitor& visitor) {
    lasterror.clear();
    Role role = session.getrole();
    if (role != Role::visitor && role != Role::admin) {
        lasterror = "请先登录";
        return false;
    }
    if (role == Role::visitor) {
        int currentindex = findvisitor(session.getuserid());
        if (currentindex == -1 || !visitors[currentindex].getactive()) {
            lasterror = "当前游客账号无效";
            return false;
        }
        if (session.getuserid() != visitorid) {
            lasterror = "只能查询本人的资料";
            return false;
        }
    }
    int index = findvisitor(visitorid);
    if (index == -1) {
        lasterror = "游客不存在";
        return false;
    }
    visitor = visitors[index];
    return true;
}
///修改昵称
bool VisitorManager::changenickname(Session& session,
    const string& newname) {
    lasterror.clear();
    if (session.getrole() != Role::visitor) {
        lasterror = "请使用游客账号登录";
        return false;
    }
    string id = session.getuserid();
    int index = findvisitor(id);
    if (index == -1 || !visitors[index].getactive()) {
        lasterror = "当前游客账号无效";
        return false;
    }
    if (!FieldValidation::validtext(newname)) {
        lasterror = "昵称不能为空白，不能包含竖线或换行";
        return false;
    }
    visitors[index].setnickname(newname);
    session = Session(Role::visitor, id, newname);
    return true;
}
///修改密码
bool VisitorManager::changepassword(const Session& session,
    const string& oldpassword,
    const string& newpassword) {
    lasterror.clear();
    if (session.getrole() != Role::visitor) {
        lasterror = "请使用游客账号登录";
        return false;
    }
    int index = findvisitor(session.getuserid());
    if (index == -1 || !visitors[index].getactive()) {
        lasterror = "当前游客账号无效";
        return false;
    }
    if (visitors[index].getpassword() != oldpassword) {
        lasterror = "原密码错误";
        return false;
    }
    if (newpassword.size() < 6 || newpassword.size() > 32) {
        lasterror = "密码长度必须为6至32个字节";
        return false;
    }
    if (!FieldValidation::validid(newpassword)) {
        lasterror = "密码不能包含空白或竖线";
        return false;
    }
    visitors[index].setpassword(newpassword);
    return true;
}
///管理员开通或取消会员
bool VisitorManager::setmembership(const Session& session,
    const string& visitorid,
    bool member) {
    lasterror.clear();
    if (session.getrole() != Role::admin) {
        lasterror = "只有管理员可以管理会员";
        return false;
    }
    int index = findvisitor(visitorid);
    if (index == -1) {
        lasterror = "游客不存在";
        return false;
    }
    if (!visitors[index].getactive()) {
        lasterror = "该游客账号已注销";
        return false;
    }
    visitors[index].setmember(member);
    return true;
}
///处理账号注销

///保存游客资料
bool VisitorManager::savetofile(const string& filename) {
    lasterror.clear();
    for (size_t i = 0; i < visitors.size(); i++) {
        const Visitor& visitor = visitors[i];
        if (!FieldValidation::validid(visitor.getvisitorid())
            || !FieldValidation::validid(visitor.getusername())
            || !FieldValidation::validid(visitor.getpassword())
            || !FieldValidation::validtext(visitor.getnickname())) {
            lasterror = "游客资料包含非法字段，无法保存";
            return false;
        }
    }
    ofstream file(filename);
    if (!file.is_open()) {
        lasterror = "无法打开游客文件";
        return false;
    }
    for (size_t i = 0; i < visitors.size(); i++) {
        const Visitor& visitor = visitors[i];
        file << visitor.getvisitorid() << "|"
            << visitor.getusername() << "|"
            << visitor.getpassword() << "|"
            << visitor.getnickname() << "|"
            << visitor.getmember() << "|"
            << visitor.getactive() << "\n";
    }
    file.close();
    if (file.fail()) {
        lasterror = "游客文件写入失败";
        return false;
    }
    return true;
}
///游客资料读取
bool VisitorManager::loadfromfile(const string& filename) {
    lasterror.clear();
    ifstream file(filename);
    if (!file.is_open()) {
        lasterror = "无法打开游客文件";
        return false;
    }
    vector<Visitor> loaded;
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
        if (fields.size() != 6 || line.back() == '|') {
            lasterror = "第" + to_string(linenumber) + "行字段数量错误";
            return false;
        }
        if (!FieldValidation::validid(fields[0])
            || !FieldValidation::validid(fields[1])
            || !FieldValidation::validid(fields[2])
            || !FieldValidation::validtext(fields[3])) {
            lasterror = "第" + to_string(linenumber) + "行包含非法字段";
            return false;
        }
        if (fields[1] == "admin") {
            lasterror = "第" + to_string(linenumber) + "行使用了管理员保留用户名";
            return false;
        }
        if (fields[2].size() < 6 || fields[2].size() > 32) {
            lasterror = "第" + to_string(linenumber) + "行密码长度错误";
            return false;
        }
        if ((fields[4] != "0" && fields[4] != "1")
            || (fields[5] != "0" && fields[5] != "1")) {
            lasterror = "第" + to_string(linenumber) + "行状态只能为0或1";
            return false;
        }
        for (size_t i = 0; i < loaded.size(); i++) {
            if (loaded[i].getvisitorid() == fields[0]) {
                lasterror = "第" + to_string(linenumber) + "行游客编号重复";
                return false;
            }
            if (loaded[i].getusername() == fields[1]) {
                lasterror = "第" + to_string(linenumber) + "行用户名重复";
                return false;
            }
        }
        bool member = fields[4] == "1";
        bool active = fields[5] == "1";
        Visitor visitor(fields[0], fields[1], fields[2], fields[3], member, active);
        loaded.push_back(visitor);
    }
    if (file.bad() || !file.eof()) {
        lasterror = "读取游客文件失败";
        return false;
    }
    visitors = loaded;
    return true;
}

string VisitorManager::getlasterror() const {
    return lasterror;
}
bool VisitorManager::closeaccount(Session& session,
    const string& password,
    const TicketManager& tickets) {
    lasterror.clear();
    if (session.getrole() != Role::visitor) {
        lasterror = "请使用游客账号注销";
        return false;
    }
    int index = findvisitor(session.getuserid());
    if (index == -1 || !visitors[index].getactive()) {
        lasterror = "当前游客账号无效";
        return false;
    }
    if (visitors[index].getpassword() != password) {
        lasterror = "密码错误";
        return false;
    }
    string today = DateUtils::today();
    if (today.empty()) {
        lasterror = "无法获取当前日期";
        return false;
    }
    if (tickets.haspendingtickets(session.getuserid(), today)) {
        lasterror = "仍有当天或未来未处理的有效门票，不能注销";
        return false;
    }
    visitors[index].setactive(false);
    session.logout();
    return true;
}
