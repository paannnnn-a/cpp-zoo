#include "visitor/ticketmanager.h"
#include "visitor/visitorutils.h"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <climits>
using namespace std;

///构造、查找每日容量和余票
TicketManager::TicketManager(VisitorManager& visitormanager,
    TicketCatalog& ticketcatalog,
    CouponManager& couponmanager)
    : visitors(visitormanager), catalog(ticketcatalog), coupons(couponmanager) {
    defaultcapacity = 1000;
}
int TicketManager::findorder(const string& orderid) const {
    for (size_t i = 0; i < orders.size(); i++) {
        if (orders[i].orderid == orderid) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
int TicketManager::finditem(const string& itemid) const {
    for (size_t i = 0; i < items.size(); i++) {
        if (items[i].itemid == itemid) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
int TicketManager::findticket(const string& ticketid) const {
    for (size_t i = 0; i < tickets.size(); i++) {
        if (tickets[i].ticketid == ticketid) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
int TicketManager::getcapacity(const string& date) const {
    for (size_t i = 0; i < capacities.size(); i++) {
        if (capacities[i].date == date) {
            return capacities[i].capacity;
        }
    }
    return defaultcapacity;
}
int TicketManager::countreserved(const string& date) const {
    int count = 0;
    for (size_t i = 0; i < tickets.size(); i++) {
        if (tickets[i].status == TicketStatus::refunded) {
            continue;
        }
        int itemindex = finditem(tickets[i].itemid);
        if (itemindex == -1) {
            continue;
        }
        int orderindex = findorder(items[itemindex].orderid);
        if (orderindex == -1) {
            continue;
        }
        if (orders[orderindex].visitdate == date) {
            count++;
        }
    }
    return count;
}
bool TicketManager::getavailability(const string& date,
    AvailabilityInfo& info) {
    lasterror.clear();
    if (!DateUtils::validdate(date)) {
        lasterror = "请输入合法的YYYY-MM-DD日期";
        return false;
    }
    AvailabilityInfo result;
    result.visitdate = date;
    result.capacity = getcapacity(date);
    result.sold = countreserved(date);
    if (result.sold > result.capacity) {
        lasterror = "当日已售票数超过容量，请检查数据";
        return false;
    }
    result.remaining = result.capacity - result.sold;
    info = result;
    return true;
}
bool TicketManager::setdailycapacity(const Session& session,
    const string& date,
    int capacity) {
    lasterror.clear();
    if (session.getrole() != Role::admin) {
        lasterror = "只有管理员可以设置每日容量";
        return false;
    }
    if (!DateUtils::validdate(date)) {
        lasterror = "请输入合法的YYYY-MM-DD日期";
        return false;
    }
    if (capacity < 0) {
        lasterror = "每日容量不能为负数";
        return false;
    }
    if (capacity < countreserved(date)) {
        lasterror = "每日容量不能少于当日未退款的已售票数";
        return false;
    }
    for (size_t i = 0; i < capacities.size(); i++) {
        if (capacities[i].date == date) {
            capacities[i].capacity = capacity;
            return true;
        }
    }
    DailyCapacity setting;
    setting.date = date;
    setting.capacity = capacity;
    capacities.push_back(setting);
    return true;
}
bool TicketManager::idexists(const string& id) const {
    for (size_t i = 0; i < orders.size(); i++) {
        if (orders[i].orderid == id) {
            return true;
        }
    }
    for (size_t i = 0; i < items.size(); i++) {
        if (items[i].itemid == id) {
            return true;
        }
    }
    for (size_t i = 0; i < tickets.size(); i++) {
        if (tickets[i].ticketid == id) {
            return true;
        }
    }
    for (size_t i = 0; i < refunds.size(); i++) {
        if (refunds[i].recordid == id || refunds[i].batchid == id) {
            return true;
        }
    }
    for (size_t i = 0; i < checkins.size(); i++) {
        if (checkins[i].recordid == id || checkins[i].batchid == id) {
            return true;
        }
    }
    return false;
}
string TicketManager::generateid(const string& prefix,
    const vector<string>& reserved) const {
    for (int number = 1; number <= 99999999; number++) {
        ostringstream result;
        result << prefix << setfill('0') << setw(4) << number;
        string id = result.str();
        if (idexists(id)) {
            continue;
        }
        bool used = false;
        for (size_t i = 0; i < reserved.size(); i++) {
            if (reserved[i] == id) {
                used = true;
                break;
            }
        }
        if (!used) {
            return id;
        }
    }
    return "";
}
bool TicketManager::checkvisitor(const Session& session,
    Visitor& visitor) {
    if (session.getrole() != Role::visitor) {
        lasterror = "请使用游客账号购票或管理个人订单";
        return false;
    }
    if (!visitors.getvisitorinfo(session, session.getuserid(), visitor)) {
        lasterror = visitors.getlasterror();
        return false;
    }
    return true;
}
bool TicketManager::previewpurchase(const Session& session,
    const PurchaseRequest& request,
    PurchasePreview& preview) {
    lasterror.clear();
    Visitor visitor;
    if (!checkvisitor(session, visitor)) {
        return false;
    }
    if (!DateUtils::validdate(request.visitdate)) {
        lasterror = "游玩日期必须是合法的YYYY-MM-DD日期";
        return false;
    }
    string today = DateUtils::today();
    if (today.empty()) {
        lasterror = "无法获取当前日期";
        return false;
    }
    string lastdate = DateUtils::adddays(today, 30);
    if (lastdate.empty()) {
        lasterror = "无法计算可售日期范围";
        return false;
    }
    if (request.visitdate < today || request.visitdate > lastdate) {
        lasterror = "只能购买今天至未来第30天的门票";
        return false;
    }
    if (request.items.empty() || request.items.size() > 3) {
        lasterror = "请选择一至三种门票";
        return false;
    }
    bool holiday = catalog.isholiday(request.visitdate);
    if (holiday&& (request.usememberdiscount || !request.couponid.empty())) {
        lasterror = "节日门票统一八折，不能叠加会员折扣或优惠券";
        return false;
    }
    if (request.usememberdiscount && !visitor.getmember()) {
        lasterror = "当前游客不是会员";
        return false;
    }
    if (request.usememberdiscount && !request.couponid.empty()) {
        lasterror = "会员折扣不能与优惠券同时使用";
        return false;
    }
    if (request.couponid.empty()) {
        if (request.couponitemindex != -1) {
            lasterror = "未选择优惠券时不能指定抵扣明细";
            return false;
        }
    }
    else {
        if (request.couponitemindex < 0
            || static_cast<size_t>(request.couponitemindex) >= request.items.size()) {
            lasterror = "请选择优惠券抵扣的购买明细";
            return false;
        }
    }
    PurchasePreview result;
    result.holidaydiscount = holiday;
    for (size_t i = 0; i < request.items.size(); i++) {
        const PurchaseItem& item = request.items[i];
        if (item.quantity < 1 || item.quantity > 10) {
            lasterror = "每种票的购买数量必须为1至10张";
            return false;
        }
        result.quantity += item.quantity;
        if (result.quantity > 10) {
            lasterror = "一笔订单最多购买10张门票";
            return false;
        }
        for (size_t j = 0; j < i; j++) {
            if (request.items[j].type == item.type) {
                lasterror = "同一种票请合并数量后购买";
                return false;
            }
        }
        TicketInfo info;
        if (!catalog.getticketinfo(item.type, info)) {
            lasterror = catalog.getlasterror();
            return false;
        }
        if (!info.onsale) {
            lasterror = info.name + "已暂停售票";
            return false;
        }
        if (info.qualification && !request.qualificationconfirmed) {
            lasterror = "请确认符合学生票或儿童票的适用资格";
            return false;
        }
        if (info.price <= 0
            || info.price > (LLONG_MAX - result.originaltotal) / item.quantity) {
            lasterror = "订单金额超出允许范围";
            return false;
        }
        result.originaltotal += info.price * item.quantity;
        Money unitprice = info.price;
        if (holiday) {
            unitprice = info.price / 10 * 8 + info.price % 10 * 8 / 10;
        }
        else if (request.usememberdiscount) {
            unitprice = info.price / 10 * 9 + info.price % 10 * 9 / 10;
        }
        Money subtotal = unitprice * item.quantity;
        if (!request.couponid.empty()
            && static_cast<size_t>(request.couponitemindex) == i) {
            Coupon coupon;
            if (!coupons.checkcoupon(session, request.couponid, info.price, coupon)) {
                lasterror = coupons.getlasterror();
                return false;
            }
            subtotal -= coupon.amount;
        }
        result.itemtotals.push_back(subtotal);
        result.totalprice += subtotal;
    }
    AvailabilityInfo availability;
    if (!getavailability(request.visitdate, availability)) {
        return false;
    }
    if (result.quantity > availability.remaining) {
        lasterror = "所选日期剩余门票不足";
        return false;
    }
    result.discountamount = result.originaltotal - result.totalprice;
    preview = result;
    return true;
}
///实现正式购票和独立出票
bool TicketManager::buytickets(const Session& session,
    const PurchaseRequest& request,
    string& orderid) {
    lasterror.clear();
    PurchasePreview preview;
    if (!previewpurchase(session, request, preview)) {
        return false;
    }
    string createdat = DateUtils::now();
    if (createdat.empty()) {
        lasterror = "无法获取当前时间";
        return false;
    }
    string today = createdat.substr(0, 10);
    string lastdate = DateUtils::adddays(today, 30);
    if (lastdate.empty()
        || request.visitdate < today
        || request.visitdate > lastdate) {
        lasterror = "当前日期已变化，请重新确认游玩日期";
        return false;
    }
    Coupon selectedcoupon;
    if (!request.couponid.empty()) {
        if (!coupons.getcouponinfo(session, request.couponid, selectedcoupon)) {
            lasterror = coupons.getlasterror();
            return false;
        }
    }
    vector<string> reserved;
    string neworderid = generateid("O", reserved);
    if (neworderid.empty()) {
        lasterror = "无法生成订单编号";
        return false;
    }
    reserved.push_back(neworderid);
    Order order;
    order.orderid = neworderid;
    order.visitorid = session.getuserid();
    order.visitdate = request.visitdate;
    order.createdat = createdat;
    order.totalprice = preview.totalprice;
    order.memberdiscount = request.usememberdiscount;
    order.holidaydiscount = preview.holidaydiscount;
    vector<Order> neworders = orders;
    vector<OrderItem> newitems = items;
    vector<AdmissionTicket> newtickets = tickets;
    string couponticketid;
    Money actualtotal = 0;
    // 准备购买明细和每张电子票
    for (size_t i = 0; i < request.items.size(); i++) {
        const PurchaseItem& selection = request.items[i];
        TicketInfo info;
        if (!catalog.getticketinfo(selection.type, info)) {
            lasterror = catalog.getlasterror();
            return false;
        }
        string itemid = generateid("I", reserved);
        if (itemid.empty()) {
            lasterror = "无法生成购买明细编号";
            return false;
        }
        reserved.push_back(itemid);
        OrderItem item;
        item.itemid = itemid;
        item.orderid = neworderid;
        item.type = selection.type;
        item.quantity = selection.quantity;
        item.unitprice = info.price;
        newitems.push_back(item);
        Money unitprice = info.price;
        if (preview.holidaydiscount) {
            unitprice = info.price / 10 * 8 + info.price % 10 * 8 / 10;
        }
        else if (request.usememberdiscount) {
            unitprice = info.price / 10 * 9 + info.price % 10 * 9 / 10;
        }
        for (int j = 0; j < selection.quantity; j++) {
            string ticketid = generateid("T", reserved);
            if (ticketid.empty()) {
                lasterror = "无法生成电子票编号";
                return false;
            }
            reserved.push_back(ticketid);
            AdmissionTicket ticket;
            ticket.ticketid = ticketid;
            ticket.itemid = itemid;
            ticket.paidprice = unitprice;
            if (!request.couponid.empty()
                && static_cast<size_t>(request.couponitemindex) == i
                && j == 0) {
                if (selectedcoupon.amount > ticket.paidprice) {
                    lasterror = "优惠券金额超过单张票价";
                    return false;
                }
                ticket.paidprice -= selectedcoupon.amount;
                ticket.couponid = request.couponid;
                couponticketid = ticketid;
            }
            if (ticket.paidprice > LLONG_MAX - actualtotal) {
                lasterror = "订单金额超出允许范围";
                return false;
            }
            actualtotal += ticket.paidprice;
            newtickets.push_back(ticket);
        }
    }
    if (actualtotal != preview.totalprice) {
        lasterror = "实际出票金额与预览不一致，请重新确认";
        return false;
    }
    neworders.push_back(order);
    // 记录本次优惠券使用结果
    if (!request.couponid.empty()) {
        if (!coupons.usecoupon(session, request.couponid, couponticketid)) {
            lasterror = coupons.getlasterror();
            return false;
        }
    }
    // 将准备好的记录统一替换到管理器
    orders.swap(neworders);
    items.swap(newitems);
    tickets.swap(newtickets);
    orderid = neworderid;
    return true;
}
///订单汇总
OrderSummary TicketManager::buildsummary(const Order& order) const {
    OrderSummary result;
    result.order = order;
    for (size_t i = 0; i < tickets.size(); i++) {
        int itemindex = finditem(tickets[i].itemid);
        if (itemindex == -1 || items[itemindex].orderid != order.orderid) {
            continue;
        }
        result.totalquantity++;
        if (tickets[i].status == TicketStatus::unused) {
            result.unusedquantity++;
        }
        else if (tickets[i].status == TicketStatus::used) {
            result.usedquantity++;
        }
        else if (tickets[i].status == TicketStatus::refunded) {
            result.refundedquantity++;
        }
        for (size_t j = 0; j < refunds.size(); j++) {
            if (refunds[j].ticketid == tickets[i].ticketid) {
                result.refundedamount += refunds[j].amount;
            }
        }
    }
    return result;
}
///查询订单列表
bool TicketManager::getorders(const Session& session,
    const OrderQuery& query,
    vector<OrderSummary>& result) {
    lasterror.clear();
    Role role = session.getrole();
    if (role != Role::visitor && role != Role::admin) {
        lasterror = "请先登录";
        return false;
    }
    if (role == Role::visitor) {
        Visitor visitor;
        if (!checkvisitor(session, visitor)) {
            return false;
        }
        if (!query.visitorid.empty()
            && query.visitorid != session.getuserid()) {
            lasterror = "只能查询本人的订单";
            return false;
        }
    }
    if (!query.visitorid.empty()
        && !FieldValidation::validid(query.visitorid)) {
        lasterror = "游客编号格式错误";
        return false;
    }
    if (!query.purchasedate.empty()
        && !DateUtils::validdate(query.purchasedate)) {
        lasterror = "购买日期格式错误";
        return false;
    }
    if (!query.visitdate.empty()
        && !DateUtils::validdate(query.visitdate)) {
        lasterror = "游玩日期格式错误";
        return false;
    }
    if (query.filterstatus
        && query.status != TicketStatus::unused
        && query.status != TicketStatus::used
        && query.status != TicketStatus::refunded) {
        lasterror = "电子票状态错误";
        return false;
    }
    vector<OrderSummary> found;
    for (size_t i = 0; i < orders.size(); i++) {
        if (role == Role::visitor
            && orders[i].visitorid != session.getuserid()) {
            continue;
        }
        if (!query.visitorid.empty()
            && orders[i].visitorid != query.visitorid) {
            continue;
        }
        if (!query.purchasedate.empty()
            && orders[i].createdat.substr(0, 10) != query.purchasedate) {
            continue;
        }
        if (!query.visitdate.empty()
            && orders[i].visitdate != query.visitdate) {
            continue;
        }
        OrderSummary summary = buildsummary(orders[i]);
        if (query.filterstatus) {
            if ((query.status == TicketStatus::unused && summary.unusedquantity == 0)
                || (query.status == TicketStatus::used && summary.usedquantity == 0)
                || (query.status == TicketStatus::refunded && summary.refundedquantity == 0)) {
                continue;
            }
        }
        found.push_back(summary);
    }
    result = found;
    return true;
}
///查询单个订单的完整资料
bool TicketManager::getorderdetail(const Session& session,
    const string& orderid,
    OrderDetail& detail) {
    lasterror.clear();
    Role role = session.getrole();
    if (role != Role::visitor && role != Role::admin) {
        lasterror = "请先登录";
        return false;
    }
    if (role == Role::visitor) {
        Visitor visitor;
        if (!checkvisitor(session, visitor)) {
            return false;
        }
    }
    int index = findorder(orderid);
    if (index == -1) {
        lasterror = "订单不存在";
        return false;
    }
    if (role == Role::visitor
        && orders[index].visitorid != session.getuserid()) {
        lasterror = "只能查询本人的订单";
        return false;
    }
    OrderDetail result;
    result.order = orders[index];
    for (size_t i = 0; i < items.size(); i++) {
        if (items[i].orderid == orderid) {
            result.items.push_back(items[i]);
        }
    }
    for (size_t i = 0; i < tickets.size(); i++) {
        int itemindex = finditem(tickets[i].itemid);
        if (itemindex != -1 && items[itemindex].orderid == orderid) {
            result.tickets.push_back(tickets[i]);
        }
    }
    for (size_t i = 0; i < result.tickets.size(); i++) {
        for (size_t j = 0; j < refunds.size(); j++) {
            if (refunds[j].ticketid == result.tickets[i].ticketid) {
                result.refunds.push_back(refunds[j]);
            }
        }
        for (size_t j = 0; j < checkins.size(); j++) {
            if (checkins[j].ticketid == result.tickets[i].ticketid) {
                result.checkins.push_back(checkins[j]);
            }
        }
    }
    detail = result;
    return true;
}
string TicketManager::getlasterror() const {
    return lasterror;
}
