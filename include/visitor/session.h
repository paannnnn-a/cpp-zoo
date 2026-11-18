#pragma once
#include <string>
using namespace std;
/// 区分当前操作身份
enum class Role {
    guest,    ///< 未登录者
    visitor,  ///< 游客
    admin     ///< 票务管理员
};
/// 保存当前登录身份
class Session {
private:
    Role role;           ///< 当前身份
    string userid;       ///< 当前用户编号
    string displayname;  ///< 页面显示名称
public:
    /// 创建未登录状态

    Session();
    /**
     * @brief 创建登录状态，由登录验证成功后的代码调用。
     * @param userrole 游客或管理员身份。
     * @param id 用户编号。
     * @param name 显示名称。
     */

    Session(Role userrole, const string& id, const string& name);

    /// 获取当前身份
    Role getrole() const;
    /// 获取当前用户编号
    string getuserid() const;
    /// 获取显示名称
    string getdisplayname() const;
    /// 判断是否登录
    bool isloggedin() const;
    /// 清除当前身份，恢复未登录状态
    void logout();
};