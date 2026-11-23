#pragma once
#include <string>
using namespace std;

/**
 * @brief 保存一名游客的账号资料
 * @details 属性包括游客编号、用户名、密码、昵称、会员身份和账号有效状态
 * 提供构造函数初始化资料，通过读取函数获取属性，通过设置函数修改资料
 * 本类没有继承或派生关系，由游客管理器保存和管理多个对象
 * 注册、身份验证及修改权限等业务检查由相应管理类完成
 */
class Visitor {
private:
    string visitorid;     ///< 游客唯一编号
    string username;      ///< 登录用户名
    string password;  ///< 登录密码，当前按明文保存
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
    /**
    * @brief 更新当前游客对象的昵称
    * @param newname 已通过游客管理器校验的新昵称
    * @details 将参数内容赋给成员nickname，会话显示名称由管理器同步
    */
    void setnickname(const string& newname);
    /**
    * @brief 更新当前游客对象的登录密码
    * @param newpassword 已通过游客管理器校验的新密码
    * @details 将参数内容赋给成员password，当前按明文保存
    */
    void setpassword(const string& newpassword);
    /**
    * @brief 设置当前游客对象的会员身份
    * @param ismember true表示开通会员，false表示取消会员
    * @details 将参数值赋给成员member，管理员权限由游客管理器检查
    */
    void setmember(bool ismember);
    /**
    * @brief 设置当前游客账号的有效状态
    * @param isactive true表示账号有效，false表示账号无效
    * @details 将参数值赋给成员active，注销时设为false并保留游客资料
    * 注销条件检查与退出登录由游客管理器完成
    */
    void setactive(bool isactive);
};