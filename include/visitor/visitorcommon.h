#pragma once
/// 金额类型，单位为分
using Money = long long;
/// 门票种类
enum class TicketType {
    adult,    ///< 成人票
    student,  ///< 学生票
    child     ///< 儿童票
};
/// 每张电子票的状态，注意，未使用不代表还能用，需依据expirydate确定票是否过期
enum class TicketStatus {
    unused,   ///< 未使用
    used,     ///< 已使用
    refunded  ///< 已退款
};
/**
 * @brief 优惠券的使用状态
 * 记录优惠券是否已经用于购票，过期情况根据有效期截止日期判断
 */
enum class CouponStatus {
    unused,   ///< 尚未用于购票
    used      ///< 已成功用于购票
};
/// 反馈种类
enum class FeedbackType {
    suggestion,  ///< 意见建议，登陆即可提交
    review       ///< 游后评价，需有入园记录
};
/// 反馈处理状态
enum class FeedbackStatus {
    pending,     ///< 未处理
    processing,  ///< 处理中
    resolved     ///< 已处理
};
/**
 * @brief 购票时选择的一种票及其数量
 * 用于购票请求，不包含订单编号、电子票状态或实付金额
 * 数量和购票条件由票务管理器检查
 */
struct PurchaseItem {
    TicketType type = TicketType::adult;  ///< 所选票种
    int quantity = 0;                     ///< 购买数量
};