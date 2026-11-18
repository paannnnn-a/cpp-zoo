#pragma once
#include <string>
using namespace std;
class VisitorManager;
class TicketCatalog;
class CouponManager;
class TicketManager;
class FeedbackManager;
/// 统一保存和加载游客子系统的数据
class CStorage {
private:
    string directory;          ///< 数据根目录
    VisitorManager& visitors;
    TicketCatalog& catalog;
    CouponManager& coupons;
    TicketManager& tickets;
    FeedbackManager& feedback;
    string lasterror;
    /// 检查优惠券所属游客及对应电子票
    bool checklinks(VisitorManager& loadedvisitors,
        CouponManager& loadedcoupons,
        TicketManager& loadedtickets);
public:
    /// 指定数据目录并关联各管理器
    CStorage(const string& datadirectory,
        VisitorManager& visitormanager,
        TicketCatalog& ticketcatalog,
        CouponManager& couponmanager,
        TicketManager& ticketmanager,
        FeedbackManager& feedbackmanager);
    /// 加载完整数据，全部检查通过后更新各管理器
    bool load();
    /// 按顺序保存各管理器的数据
    bool save();
    /// 获取最近一次操作的错误说明
    string getlasterror() const;
};