#include "visitor/visitormenu.h"
#include <iostream>
using namespace std;
void VisitorMenu::registervisitor() {
    string username;
    string password;
    string nickname;
    if (!readtext("请输入用户名", username)) {
        return;
    }
    if (!readtext("请输入密码，长度为6至32字节", password)) {
        return;
    }
    if (!readtext("请输入昵称", nickname)) {
        return;
    }
    string visitorid;
    bool success = visitors.registervisitor(
        username, password, nickname, visitorid);
    finishoperation(session, "REGISTER", visitorid,
        success, visitors.getlasterror());
    if (success) {
        cout << "请通过登录菜单登录账号\n";
    }
}
void VisitorMenu::login() {
    string username;
    string password;
    if (!readtext("请输入用户名", username)) {
        return;
    }
    if (!readtext("请输入密码", password)) {
        return;
    }
    bool success = auth.login(username, password, session);
    string description;
    if (success) {
        description = "登录成功，欢迎" + session.getdisplayname();
    }
    else {
        description = auth.getlasterror();
    }
    cout << description << "\n";
    if (!logger.writelog(session, "LOGIN", "", success, description)) {
        cout << "日志写入失败：" << logger.getlasterror() << "\n";
    }
}
void VisitorMenu::logout() {
    Session actor = session;
    auth.logout(session);
    cout << "已退出登录\n";
    if (!logger.writelog(actor, "LOGOUT", "", true, "退出登录")) {
        cout << "日志写入失败：" << logger.getlasterror() << "\n";
    }
}
///个人账号菜单
char VisitorMenu::accountmenu() {
    while (session.getrole() == Role::visitor) {
        cout << "\n========== 个人账号 ==========\n";
        cout << "当前用户：" << session.getdisplayname() << "\n";
        cout << "1. 查看个人资料\n";
        cout << "2. 修改昵称\n";
        cout << "3. 修改密码\n";
        cout << "4. 注销账号\n";
        cout << "0. 返回上一级\n";
        cout << "M. 返回系统主菜单\n";
        cout << "Q. 请求退出系统\n";
        cout << "请选择：";
        string choice;
        if (!getline(cin, choice)) {
            return 'Q';
        }
        if (choice == "0") {
            return '0';
        }
        if (choice == "M" || choice == "m") {
            return 'M';
        }
        if (choice == "Q" || choice == "q") {
            return 'Q';
        }
        if (choice == "1") {
            showprofile();
        }
        else if (choice == "2") {
            changenickname();
        }
        else if (choice == "3") {
            changepassword();
        }
        else if (choice == "4") {
            closeaccount();
        }
        else {
            cout << "无效选项，请重新输入\n";
        }
    }
    return '0';
}
void VisitorMenu::showprofile() {
    Visitor visitor;
    if (!visitors.getvisitorinfo(
        session, session.getuserid(), visitor)) {
        cout << visitors.getlasterror() << "\n";
        return;
    }
    cout << "\n========== 个人资料 ==========\n";
    cout << "游客编号：" << visitor.getvisitorid() << "\n";
    cout << "用户名：" << visitor.getusername() << "\n";
    cout << "昵称：" << visitor.getnickname() << "\n";
    cout << "会员身份：";
    if (visitor.getmember()) {
        cout << "会员，购票可选择会员九折优惠\n";
    }
    else {
        cout << "普通游客\n";
    }
}
///修改昵称、密码和注销账号
void VisitorMenu::changenickname() {
    string nickname;
    if (!readtext("请输入新昵称", nickname)) {
        return;
    }
    Session actor = session;
    bool success = visitors.changenickname(session, nickname);
    finishoperation(actor, "CHANGE_NICKNAME", actor.getuserid(),
        success, visitors.getlasterror());
}
void VisitorMenu::changepassword() {
    string oldpassword;
    string newpassword;
    if (!readtext("请输入原密码", oldpassword)) {
        return;
    }
    if (!readtext("请输入新密码，长度为6至32字节", newpassword)) {
        return;
    }
    Session actor = session;
    bool success = visitors.changepassword(
        session, oldpassword, newpassword);
    finishoperation(actor, "CHANGE_PASSWORD", actor.getuserid(),
        success, visitors.getlasterror());
}
void VisitorMenu::closeaccount() {
    string password;
    if (!readtext("请输入密码验证身份", password)) {
        return;
    }
    if (!confirm("注销后账号将停用，历史记录保留，是否继续")) {
        return;
    }
    Session actor = session;
    bool success = visitors.closeaccount(session, password, tickets);
    finishoperation(actor, "CLOSE_ACCOUNT", actor.getuserid(),
        success, visitors.getlasterror());
}