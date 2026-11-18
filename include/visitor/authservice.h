#pragma once
#include <string>
#include "visitormanager.h"
#include "session.h"
using namespace std;
/// 处理游客和管理员的登录与退出
class AuthService {
private:
    VisitorManager& visitors;  ///< 共用的游客管理器
    string lasterror;         ///< 最近一次操作的错误说明
public:
    /// 关联游客管理器
    AuthService(VisitorManager& manager);
    /**
     * @brief 验证账号并建立登录状态
     * @param username 用户名
     * @param password 密码
     * @param session 当前登录状态
     * @return 登录成功返回 true
     */
    bool login(const string& username,
        const string& password,
        Session& session);
    /// 退出登录
    void logout(Session& session);
    /// 获取最近一次操作的错误说明
    string getlasterror() const;
};