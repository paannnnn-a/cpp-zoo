#pragma once
#include <string>
using namespace std;
/**
 * @brief C模块当前操作者的身份
 * 用于区分未登录者、游客和票务管理员，会员身份由游客资料单独保存
 */
enum class Role {
    guest,    ///< 未登录者
    visitor,  ///< 游客
    admin     ///< 票务管理员
};
/**
 * @brief 保存C模块当前登录会话
 * @details 属性包括操作身份、用户编号和页面显示名称
 * 提供会话初始化、身份读取、登录状态判断和退出登录方法
 * 本类没有继承或派生关系，由游客菜单持有，登录服务负责验证后更新
 * 业务管理器通过会话身份和用户编号检查操作权限
 */
class Session {
private:
    Role role;           ///< 当前身份
    string userid;       ///< 当前用户编号
    string displayname;  ///< 页面显示名称
public:
    /**
    * @brief 创建未登录会话
    * @details 将身份设为guest，用户编号和显示名称设为空字符串
    */

    Session();
    /**
    * @brief 根据已验证的身份信息创建会话
    * @param userrole 当前身份，游客使用visitor，票务管理员使用admin
    * @param id 当前用户编号，游客使用游客编号，管理员使用ADMIN
    * @param name 页面显示名称，游客使用昵称
    * @details 将三个参数分别保存到身份、用户编号和显示名称成员
    */

    Session(Role userrole, const string& id, const string& name);

    /// 获取当前身份
    Role getrole() const;
    /// 获取当前用户编号
    string getuserid() const;
    /// 获取显示名称
    string getdisplayname() const;
    /**
    * @brief 判断当前会话是否已经登录
    * @return 游客或管理员身份返回true，未登录身份返回false
    * @details 将成员role与Role::guest比较，不进行账号和密码验证
    */
    bool isloggedin() const;
    /**
    * @brief 退出当前登录会话
    * @details 将身份设为guest，清空用户编号和显示名称
    * 保留游客账号资料及已有业务记录
    */
    void logout();
};