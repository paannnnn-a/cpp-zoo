#include "visitor/session.h"
using namespace std;
// 默认没有用户登录
Session::Session() {
    role = Role::guest;
    userid = "";
    displayname = "";
}
// 保存登录验证成功后的身份
Session::Session(Role userrole, const string& id, const string& name) {
    role = userrole;
    userid = id;
    displayname = name;
}
// 获取当前身份
Role Session::getrole() const {
    return role;
}
// 获取当前用户编号
string Session::getuserid() const {
    return userid;
}
// 获取显示名称
string Session::getdisplayname() const {
    return displayname;
}
// 判断当前是否已经登录
bool Session::isloggedin() const {
    return role != Role::guest;
}
// 退出   登录并清空当前身份
void Session::logout() {
    role = Role::guest;
    userid = "";
    displayname = "";
}