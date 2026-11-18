#pragma once
#include <string>
#include <vector>
#include "visitorcommon.h"
using namespace std;
/// 一次购票的订单
struct Order {
    string orderid;               ///< 订单编号
    string visitorid;             ///< 所属游客编号
    string visitdate;             ///< 游玩日期
    string createdat;             ///< 购买时间
    Money totalprice = 0;         ///< 实际付款总额，单位为分
    bool memberdiscount = false;  ///< 本次是否使用会员折扣
    bool holidaydiscount = false; ///< 购买时是否使用节日八折
};
/// 订单中一种票的购买明细
struct OrderItem {
    string itemid;                       ///< 明细编号
    string orderid;                      ///< 所属订单编号
    TicketType type = TicketType::adult;  ///< 票种
    int quantity = 0;                    ///< 购买数量
    Money unitprice = 0;                 ///< 购买时的基础单价，单位为分
};
/// 一张可以独立检票和退款的电子票
struct AdmissionTicket {
    string ticketid;                             ///< 电子票编号
    string itemid;                               ///< 所属明细编号
    Money paidprice = 0;                         ///< 本张票实际付款金额，单位为分
    TicketStatus status = TicketStatus::unused;  ///< 当前状态
    string couponid;                            ///< 使用的优惠券编号，未使用则为空
};
/// 一张电子票的退款记录
struct RefundRecord {
    string recordid;    ///< 退款记录编号
    string batchid;     ///< 本次退款操作的批次编号
    string ticketid;    ///< 被退款的电子票编号
    string operatorid;  ///< 操作人编号
    Money amount = 0;   ///< 实际退款金额，单位为分
    string refundedat;  ///< 退款时间
};
/// 一张电子票的检票记录
struct CheckInRecord {
    string recordid;    ///< 检票记录编号
    string batchid;     ///< 本次检票操作的批次编号
    string ticketid;    ///< 被检票的电子票编号
    string operatorid;  ///< 操作人编号
    string checkedinat; ///< 检票时间
};
/// 订单列表中的汇总资料
struct OrderSummary {
    Order order;                 ///< 订单资料
    int totalquantity = 0;       ///< 购买总张数
    int unusedquantity = 0;      ///< 未使用张数
    int usedquantity = 0;        ///< 已使用张数
    int refundedquantity = 0;    ///< 已退款张数
    Money refundedamount = 0;    ///< 已退款金额，单位为分
};
/// 单个订单的完整查询结果
struct OrderDetail {
    Order order;                       ///< 订单资料
    vector<OrderItem> items;            ///< 购买明细
    vector<AdmissionTicket> tickets;    ///< 全部电子票
    vector<RefundRecord> refunds;       ///< 退款记录
    vector<CheckInRecord> checkins;     ///< 检票记录
};
