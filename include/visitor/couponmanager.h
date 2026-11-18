#pragma once
#include <string>
#include <vector>
#include "visitorcommon.h"
#include "session.h"
#include "visitormanager.h"
using namespace std;
/// 一张优惠券的资料
struct Coupon {
    string couponid;                         ///< 优惠券编号
    string visitorid;                        ///< 所属游客编号
    Money amount = 500;                      ///< 抵扣金额，单位为分
    string expirydate;                       ///< 有效期截止日期
    string issuedat;                         ///< 发放时间
    CouponStatus status = CouponStatus::unused; ///< 使用状态
    string ticketid;                         ///< 使用优惠券的电子票编号
    string usedat;                           ///< 使用时间
};
/// 管理优惠券发放、查询、使用及文件读写
class CouponManager {
    friend class CStorage;
private:
    VisitorManager& visitors;  ///< 共用的游客管理器
    vector<Coupon> coupons;    ///< 全部优惠券
    string lasterror;         ///< 最近一次操作的错误说明
    /// 按优惠券编号查找下标，未找到返回 -1
    int findcoupon(const string& couponid) const;
    /// 生成未使用的优惠券编号
    string generateid() const;
public:
    /// 关联游客管理器
    CouponManager(VisitorManager& manager);
    /**
     * @brief 管理员向有效游客账号发放5元券
     * @param expirydate 有效期截止日期，格式为 YYYY-MM-DD
     * @param couponid 成功时接收优惠券编号
     * @return 发放成功返回 true
     */
    bool issuecoupon(const Session& session,
        const string& visitorid,
        const string& expirydate,
        string& couponid);
    /// 游客查询本人的优惠券，管理员查询全部优惠券
    bool getcoupons(const Session& session, vector<Coupon>& result);
    /// 查询指定优惠券，游客限本人，管理员可查询全部
    bool getcouponinfo(const Session& session,
        const string& couponid,
        Coupon& coupon);
    /**
     * @brief 检查当前游客能否将优惠券用于指定金额的门票
     * @param ticketprice 优惠前的单张票金额，单位为分
     * @param coupon 成功时接收优惠券资料
     * @return 符合使用条件返回 true
     */
    bool checkcoupon(const Session& session,
        const string& couponid,
        Money ticketprice,
        Coupon& coupon);
    /// 购票流程确认后记录优惠券使用状态和电子票编号
    bool usecoupon(const Session& session,
        const string& couponid,
        const string& ticketid);
    /// 加载优惠券资料
    bool loadfromfile(const string& filename);
    /// 保存优惠券资料
    bool savetofile(const string& filename);
    /// 获取最近一次操作的错误说明
    string getlasterror() const;
};