#pragma once
#include <string>
#include "visitorcommon.h"
using namespace std;
/// 保存票种价格、售票状态及公共接口
class Ticket {
private:
    Money price;  ///< 当前票价，单位为分
    bool onsale;  ///< 是否正在售票
public:
    /// 设置初始票价，默认正在售票
    Ticket(Money initialprice);
    virtual ~Ticket();
    /// 获取当前票价
    Money getprice() const;
    /// 设置大于零的票价，设置成功返回 true
    bool setprice(Money newprice);
    /// 获取售票状态
    bool getonsale() const;
    /// 设置售票状态
    void setonsale(bool value);
    /// 获取票种
    virtual TicketType gettype() const = 0;
    /// 获取票种名称
    virtual string getname() const = 0;
    /// 获取适用资格说明
    virtual string geteligibility() const = 0;
    /// 是否需要确认优惠资格
    virtual bool needqualification() const;
};
/// 成人票，初始价格为50元
class AdultTicket : public Ticket {
public:
    AdultTicket();
    TicketType gettype() const override;
    string getname() const override;
    string geteligibility() const override;
};
/// 需要确认适用资格的优惠票基类
class DiscountTicket : public Ticket {
public:
    DiscountTicket(Money initialprice);
    bool needqualification() const override;
};
/// 学生票，初始价格为25元
class StudentTicket : public DiscountTicket {
public:
    StudentTicket();
    TicketType gettype() const override;
    string getname() const override;
    string geteligibility() const override;
};
/// 儿童票，初始价格为20元
class ChildTicket : public DiscountTicket {
public:
    ChildTicket();
    TicketType gettype() const override;
    string getname() const override;
    string geteligibility() const override;
};