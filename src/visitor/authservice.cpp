#include "visitor/authservice.h"
using namespace std;
AuthService::AuthService(VisitorManager& manager)
    : visitors(manager) {
}
bool AuthService::login(const string& username,
    const string& password,
    Session& session) {
    lasterror.clear();
    // 已登录时先退出当前账号
    if (session.isloggedin()) {
        lasterror = "请先退出当前账号";
        return false;
    }
    // 验证预置管理员账号
    if (username == "admin") {
        if (password != "admin1234") {
            lasterror = "用户名或密码错误";
            return false;
        }
        session = Session(Role::admin, "ADMIN", "票务管理员");
        return true;
    }
    // 验证游客账号，失败时取得具体原因
    Visitor visitor;
    if (!visitors.authenticate(username, password, visitor)) {
        lasterror = visitors.getlasterror();
        return false;
    }
    // 使用游客编号和昵称建立登录状态
    session = Session(Role::visitor,
        visitor.getvisitorid(),
        visitor.getnickname());
    return true;
}
void AuthService::logout(Session& session) {
    session.logout();
    lasterror.clear();
}
string AuthService::getlasterror() const {
    return lasterror;
}