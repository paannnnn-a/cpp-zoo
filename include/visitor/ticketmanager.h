#pragma once
#include "order.h"
#include "session.h"
#include "visitormanager.h"
#include "ticketcatalog.h"
#include "couponmanager.h"
#include <string>
#include <vector>
using namespace std;
/// 一次购票的输入资料
struct PurchaseRequest {
    string visitdate;                      ///< 游玩日期
    vector<PurchaseItem> items;             ///< 票种及数量
    bool usememberdiscount = false;        ///< 是否使用会员九折
    string couponid;                       ///< 优惠券编号，不使用则为空
    int couponitemindex = -1;              ///< 优惠券用于哪条购买明细
    bool qualificationconfirmed = false;   ///< 是否已确认学生或儿童票资格
};
/// 确认购买前的金额预览
struct PurchasePreview {
    int quantity = 0;          ///< 总张数
    Money originaltotal = 0;   ///< 基础价格合计，单位为分
    Money discountamount = 0;  ///< 优惠金额合计，单位为分
    Money totalprice = 0;      ///< 实际付款总额，单位为分
    bool holidaydiscount = false; ///< 本次预览是否使用节日八折
    vector<Money> itemtotals;  ///< 各条明细的实际付款金额
};
/// 指定游玩日期的售票情况
struct AvailabilityInfo {
    string visitdate;   ///< 游玩日期
    int capacity = 0;   ///< 当日容量
    int sold = 0;       ///< 未退款的已售票数
    int remaining = 0;  ///< 剩余可售票数
};
/// 指定日期的容量设置
struct DailyCapacity {
    string date;       ///< 游玩日期
    int capacity = 0;  ///< 当日容量
};
/// 订单查询条件，空日期和空游客编号表示不按该项筛选
struct OrderQuery {
    string purchasedate;                       ///< 购买日期
    string visitdate;                          ///< 游玩日期
    string visitorid;                          ///< 游客编号
    bool filterstatus = false;                ///< 是否按电子票状态筛选
    TicketStatus status = TicketStatus::unused; ///< 要查询的电子票状态
};
/// 每日运营统计
struct DailyStatistics {
    string date;                ///< 统计日期
    int capacity = 0;           ///< 当日容量
    int sold = 0;               ///< 该游玩日期未退款的票数
    int remaining = 0;          ///< 剩余可售票数
    int adultsold = 0;          ///< 成人票有效销量
    int studentsold = 0;        ///< 学生票有效销量
    int childsold = 0;          ///< 儿童票有效销量
    int checkedincount = 0;     ///< 当天检票人数
    Money revenue = 0;          ///< 当天购买订单的付款金额
    Money refundamount = 0;     ///< 当天退款金额
    Money netrevenue = 0;       ///< 当天付款减退款金额
};
/// 管理容量、购票、订单、退款、检票和运营统计
class TicketManager {
    friend class CStorage;
private:
    VisitorManager& visitors;          ///< 游客管理器
    TicketCatalog& catalog;            ///< 票种管理器
    CouponManager& coupons;            ///< 优惠券管理器
    vector<Order> orders;              ///< 全部订单
    vector<OrderItem> items;           ///< 全部购买明细
    vector<AdmissionTicket> tickets;   ///< 全部电子票
    vector<RefundRecord> refunds;      ///< 全部退款记录
    vector<CheckInRecord> checkins;     ///< 全部检票记录
    vector<DailyCapacity> capacities;  ///< 按日期设置的容量
    int defaultcapacity;              ///< 默认每日容量
    string lasterror;                 ///< 最近一次操作的错误说明
    /// 按订单编号查找下标
    int findorder(const string& orderid) const;
    /// 按明细编号查找下标
    int finditem(const string& itemid) const;
    /// 按电子票编号查找下标
    int findticket(const string& ticketid) const;
    /// 检查记录编号或批次编号是否已经使用
    bool idexists(const string& id) const;
    /// 生成编号，同时避开本次操作已经预留的编号
    string generateid(const string& prefix,
        const vector<string>& reserved) const;
    /// 检查当前游客账号并取得资料
    bool checkvisitor(const Session& session, Visitor& visitor);
    /// 获取指定日期的容量
    int getcapacity(const string& date) const;
    /// 统计指定游玩日期未退款的票数
    int countreserved(const string& date) const;
    /// 根据实际记录计算订单汇总
    OrderSummary buildsummary(const Order& order) const;
    /// 检查加载的订单关联、数量与金额
    bool validateorders();
    /// 检查加载的退款、入园记录与门票状态
    bool validateoperations();
    /// 检查加载的门票数量与接待容量
    bool validatecapacity();
public:
    /// 关联账号、票种及优惠券管理器，默认每日容量为1000
    TicketManager(VisitorManager& visitormanager,
        TicketCatalog& ticketcatalog,
        CouponManager& couponmanager);
    /// 查询指定游玩日期的容量、销量和余票
    bool getavailability(const string& date, AvailabilityInfo& info);
    /// 管理员设置指定日期的容量
    bool setdailycapacity(const Session& session,
        const string& date,
        int capacity);
    /// 检查购票条件并计算金额，不创建订单或使用优惠券
    bool previewpurchase(const Session& session,
        const PurchaseRequest& request,
        PurchasePreview& preview);
    /// 确认购买并模拟付款，生成订单、明细和独立电子票
    bool buytickets(const Session& session,
        const PurchaseRequest& request,
        string& orderid);
    /// 游客查询本人订单，管理员按条件查询全部订单
    bool getorders(const Session& session,
        const OrderQuery& query,
        vector<OrderSummary>& result);
    /// 查询单个订单的明细、电子票和操作记录
    bool getorderdetail(const Session& session,
        const string& orderid,
        OrderDetail& detail);
    /// 退还同一订单中选定的电子票
    bool refundtickets(const Session& session,
        const vector<string>& ticketids,
        string& batchid);
    /// 管理员检验同一订单中选定的电子票
    bool checkintickets(const Session& session,
        const vector<string>& ticketids,
        string& batchid);
    /// 判断游客是否仍有指定日期起未使用且未退款的门票
    bool haspendingtickets(const string& visitorid,
        const string& fromdate) const;
    /// 判断游客是否在指定游玩日期成功入园
    bool hasvisited(const string& visitorid,
        const string& visitdate) const;
    /// 管理员查询每日运营统计
    bool getdailystatistics(const Session& session,
        const string& date,
        DailyStatistics& statistics);
    /// 加载订单、明细、电子票、操作记录和容量设置
    bool loadfromfiles(const string& directory);
    /// 保存订单、明细、电子票、操作记录和容量设置
    bool savetofiles(const string& directory);
    /// 获取最近一次操作的错误说明
    string getlasterror() const;
};