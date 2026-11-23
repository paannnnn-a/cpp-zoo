#include "visitor/visitor.h"
using namespace std;

// 默认构造函数（string 成员会自动初始化为空字符串）
Visitor::Visitor() {
    member = false;
    active = false;
}

/**
 * @brief 使用传入资料创建游客对象
 * @param id 游客编号
 * @param user 登录用户名
 * @param hash 登录密码，保留现有参数名，当前不进行哈希处理
 * @param nick 显示昵称
 * @param ismember 是否为会员，省略时为false
 * @param isactive 账号是否有效，省略时为true
 */
Visitor::Visitor(const string& id,
    const string& user,
    const string& hash,
    const string& nick,
    bool ismember,
    bool isactive) {
    visitorid = id;
    username = user;
    password = hash;
    nickname = nick;
    member = ismember;
    active = isactive;
}

// 获取游客编号
string Visitor::getvisitorid() const {
    return visitorid;
}
// 获取用户名
string Visitor::getusername() const {
    return username;
}
// 获取密码验证信息
string Visitor::getpassword() const {
    return password;
}
// 获取昵称
string Visitor::getnickname() const {
    return nickname;
}
// 获取会员状态
bool Visitor::getmember() const {
    return member;
}
// 获取账号有效状态
bool Visitor::getactive() const {
    return active;
}
// 修改昵称
void Visitor::setnickname(const string& newname) {
    nickname = newname;
}
// 更新密码验证信息
void Visitor::setpassword(const string& newpassword) {
    password = newpassword;
}
// 修改会员状态
void Visitor::setmember(bool ismember) {
    member = ismember;
}
// 修改账号有效状态
void Visitor::setactive(bool isactive) {
    active = isactive;
}