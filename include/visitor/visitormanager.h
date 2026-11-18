#pragma once
#include <string>
#include <vector>
#include "visitor.h"
#include "session.h"
using namespace std;

class TicketManager;
/// 管理游客账号及文件读写
class VisitorManager {
    friend class CStorage;
private:
    vector<Visitor> visitors;  ///< 全部游客资料
    string lasterror;         ///< 最近一次操作的错误说明
    /// 按编号查找游客，返回下标，找不到返回 -1
    int findvisitor(const string& visitorid) const;
    /// 按用户名查找游客，返回下标，找不到返回 -1
    int findusername(const string& username) const;
    /// 生成未使用的游客编号
    string generateid() const;
public:
    /// 创建游客管理器
    VisitorManager();
    /**
     * @brief 注册普通游客
     * @param username 用户名
     * @param password 原始密码
     * @param nickname 昵称
     * @param visitorid 成功时接收生成的游客编号
     * @return 注册成功返回 true
     */
    bool registervisitor(const string& username,
        const string& password,
        const string& nickname,
        string& visitorid);
    /**
     * @brief 验证用户名和密码
     * @param visitor 成功时接收游客资料
     * @return 账号有效且密码正确时返回 true
     */
    bool authenticate(const string& username,
        const string& password,
        Visitor& visitor);
    /// 查询游客资料，游客查询本人，管理员可查询所有人
    bool getvisitorinfo(const Session& session,
        const string& visitorid,
        Visitor& visitor);
    /// 修改当前游客昵称并同步登录显示名称
    bool changenickname(Session& session,
        const string& newname);
    /// 验证原密码后修改当前游客密码
    bool changepassword(const Session& session,
        const string& oldpassword,
        const string& newpassword);
    /// 管理员设置指定游客的会员身份
    bool setmembership(const Session& session,
        const string& visitorid,
        bool member);
    /// 验证密码和未完成门票后注销账号并退出登录
    bool closeaccount(Session& session,
        const string& password,
        const TicketManager& tickets);
    /// 加载游客文件
    bool loadfromfile(const string& filename);
    /// 保存游客文件
    bool savetofile(const string& filename);
    /// 获取最近一次操作的错误说明
    string getlasterror() const;
};