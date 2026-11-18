#pragma once
#include <string>
#include <vector>
#include "visitormanager.h"
#include "ticketcatalog.h"
#include "couponmanager.h"
#include "ticketmanager.h"
#include "feedbackmanager.h"
#include "authservice.h"
#include "routemanager.h"
#include "cstorage.h"
#include "operationlogger.h"
using namespace std;
class ZoneManager;
class AnimalManager;
/// 提供游客与票务管理员的控制台交互
class VisitorMenu {
private:
    const ZoneManager& zones;
    const AnimalManager& animals;
    VisitorManager visitors;
    TicketCatalog catalog;
    CouponManager coupons;
    TicketManager tickets;
    FeedbackManager feedback;
    AuthService auth;
    RouteManager routes;
    CStorage storage;
    OperationLogger logger;
    Session session;
    string lasterror;
    /// 读取文本，allowempty表示是否接受空输入
    bool readtext(const string& prompt, string& text,
        bool allowempty = false);
    /// 读取指定范围内的整数
    bool readint(const string& prompt, int minimum,
        int maximum, int& value);
    /// 读取以元为单位的金额并转换为分
    bool readmoney(const string& prompt, Money& value);
    /// 读取以空格分隔的电子票编号
    bool readticketids(const string& prompt, vector<string>& ids);
    /// 确认购买、退款或账号注销
    bool confirm(const string& message);
    /// 将整数分转换为金额显示文字
    string moneytext(Money amount) const;
    /// 将票种与状态转换为显示文字
    string tickettypename(TicketType type) const;
    string ticketstatusname(TicketStatus status) const;
    string feedbacktypename(FeedbackType type) const;
    string feedbackstatusname(FeedbackStatus status) const;
    /// 显示操作结果并连接数据保存与日志
    void finishoperation(const Session& actor,
        const string& action, const string& targetid,
        bool success, const string& error = "");
    /// 各功能分组菜单
    char accountmenu();
    char ticketmenu();
    char servicemenu();
    char adminmenu();
    /// 账号操作
    void registervisitor();
    void login();
    void logout();
    void showprofile();
    void changenickname();
    void changepassword();
    void closeaccount();
    /// 票务操作
    void showprices();
    void showavailability();
    void buytickets();
    void queryorders();
    void showorderdetail();
    void refundtickets();
    void showcoupons();
    void showorder(const OrderDetail& detail) const;
    /// 游园查询与反馈
    void browseanimals();
    void showroutes();
    void submitsuggestion();
    void submitreview();
    void queryfeedback();
    void showfeedbackdetail();
    void showfeedback(const Feedback& item) const;
    void showanimal(const AnimalBasicInfo& animal) const;
    /// 管理员操作
    void updateprice();
    void setonsale();
    void setdailycapacity();
    void checkintickets();
    void showstatistics();
    void setmembership();
    void issuecoupon();
    void handlefeedback();
    /// 节日优惠管理菜单
    char holidaymenu();
    /// 显示节日规则
    void showholidays();
    /// 添加节日规则
    void addholiday();
    /// 删除选中的节日规则
    void removeholiday();
public:
    /// 连接动物园区资料并指定游客数据目录
    VisitorMenu(const ZoneManager& zonemanager,
        const AnimalManager& animalmanager,
        const string& datadirectory);
    /// 加载游客子系统数据
    bool load();
    /// 保存游客子系统数据
    bool save();
    /// 运行菜单，返回0或M回主菜单，Q请求退出
    char run();
    /// 获取最近一次加载或保存失败的说明
    string getlasterror() const;
};