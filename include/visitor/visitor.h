#pragma once
#include <string>
using namespace std;

/// 保存游客资料
class Visitor {
private:
    string visitorid;     ///< 游客唯一编号
    string username;      ///< 登录用户名
    string password;  ///< 密码处理后的验证信息
    string nickname;      ///< 显示昵称
    bool member;          ///< 是否为会员
    bool active;          ///< 账号是否有效
public:
    /// 创建空对象(资料为空，非会员，账号无效）
    Visitor();
    /**
     * @brief 使用已有资料创建游客对象。
     * @param id 游客编号。
     * @param user 用户名。
     * @param hash 密码验证信息。
     * @param nick 昵称。
     * @param ismember 是否为会员，默认 false。
     * @param isactive 账号是否有效，默认 true。
     */
    Visitor(const string& id,
        const string& user,
        const string& hash,
        const string& nick,
        bool ismember = false,
        bool isactive = true);
    /// 获取游客编号
    string getvisitorid() const;
    /// 获取用户名
    string getusername() const;
    /// 获取密码验证信息，仅供账号验证和保存使用
    string getpassword() const;
    /// 获取昵称
    string getnickname() const;
    /// 判断是否为会员
    bool getmember() const;
    /// 判断账号是否有效
    bool getactive() const;
    /// 修改昵称，内容合法性由管理类检查
    void setnickname(const string& newname);
    /// 更新密码验证信息
    void setpassword(const string& newpassword);
    /// 设置会员身份，管理员权限由管理类检查
    void setmember(bool ismember);
    /// 设置账号有效状态，注销条件由管理类检查
    void setactive(bool isactive);
};