#pragma once
/// 金额类型，单位为分
using Money = long long;
/// 门票种类
enum class TicketType {
    adult,    ///< 成人票
    student,  ///< 学生票
    child     ///< 儿童票
};
/// 每张电子票的状态
enum class TicketStatus {
    unused,   ///< 未使用
    used,     ///< 已使用
    refunded  ///< 已退款
};
/// 优惠券状态，过期情况按有效日期判断
enum class CouponStatus {
    unused,   ///< 未使用
    used      ///< 已使用
};
/// 反馈种类
enum class FeedbackType {
    suggestion,  ///< 意见建议
    review       ///< 游后评价
};
/// 反馈处理状态
enum class FeedbackStatus {
    pending,     ///< 未处理
    processing,  ///< 处理中
    resolved     ///< 已处理
};
/// 购票时选择的票种和数量
struct PurchaseItem {
    TicketType type = TicketType::adult;  ///< 所选票种
    int quantity = 0;                     ///< 购买数量
};