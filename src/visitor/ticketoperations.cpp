#include "visitor/ticketmanager.h"
#include "visitor/visitorutils.h"
#include <climits>
using namespace std;
///退款（可部分退款）和批量检票
bool TicketManager::refundtickets(const Session& session,
    const vector<string>& ticketids,
    string& batchid) {
    lasterror.clear();
    Visitor visitor;
    if (!checkvisitor(session, visitor)) {
        return false;
    }
    if (ticketids.empty() || ticketids.size() > 10) {
        lasterror = "请选择同一订单中的1至10张电子票";
        return false;
    }
    string refundedat = DateUtils::now();
    if (refundedat.empty()) {
        lasterror = "无法获取当前时间";
        return false;
    }
    string today = refundedat.substr(0, 10);
    string selectedorderid;
    vector<int> selected;
    // 检查所选门票的归属、状态和退款期限
    for (size_t i = 0; i < ticketids.size(); i++) {
        for (size_t j = 0; j < i; j++) {
            if (ticketids[i] == ticketids[j]) {
                lasterror = "不能重复选择同一张电子票";
                return false;
            }
        }
        int ticketindex = findticket(ticketids[i]);
        if (ticketindex == -1) {
            lasterror = "电子票不存在";
            return false;
        }
        if (tickets[ticketindex].status != TicketStatus::unused) {
            lasterror = "只能退还未使用且未退款的电子票";
            return false;
        }
        int itemindex = finditem(tickets[ticketindex].itemid);
        if (itemindex == -1) {
            lasterror = "电子票缺少对应的购买明细";
            return false;
        }
        int orderindex = findorder(items[itemindex].orderid);
        if (orderindex == -1) {
            lasterror = "电子票缺少对应的订单";
            return false;
        }
        const Order& order = orders[orderindex];
        if (order.visitorid != session.getuserid()) {
            lasterror = "只能退还本人的电子票";
            return false;
        }
        if (order.visitdate <= today) {
            lasterror = "只能在游玩日期当天零点前退款";
            return false;
        }
        if (refundedat < order.createdat) {
            lasterror = "当前时间早于订单购买时间";
            return false;
        }
        if (i == 0) {
            selectedorderid = order.orderid;
        }
        else if (order.orderid != selectedorderid) {
            lasterror = "一次只能退还同一订单中的电子票";
            return false;
        }
        selected.push_back(ticketindex);
    }
    vector<string> reserved;
    string newbatchid = generateid("RB", reserved);
    if (newbatchid.empty()) {
        lasterror = "无法生成退款批次编号";
        return false;
    }
    reserved.push_back(newbatchid);
    vector<AdmissionTicket> newtickets = tickets;
    vector<RefundRecord> newrefunds = refunds;
    for (size_t i = 0; i < selected.size(); i++) {
        string recordid = generateid("R", reserved);
        if (recordid.empty()) {
            lasterror = "无法生成退款记录编号";
            return false;
        }
        reserved.push_back(recordid);
        int index = selected[i];
        RefundRecord record;
        record.recordid = recordid;
        record.batchid = newbatchid;
        record.ticketid = tickets[index].ticketid;
        record.operatorid = session.getuserid();
        record.amount = tickets[index].paidprice;
        record.refundedat = refundedat;
        newrefunds.push_back(record);
        newtickets[index].status = TicketStatus::refunded;
    }
    tickets.swap(newtickets);
    refunds.swap(newrefunds);
    batchid = newbatchid;
    return true;
}
bool TicketManager::checkintickets(const Session& session,
    const vector<string>& ticketids,
    string& batchid) {
    lasterror.clear();
    if (session.getrole() != Role::admin) {
        lasterror = "只有管理员可以检票";
        return false;
    }
    if (ticketids.empty() || ticketids.size() > 10) {
        lasterror = "请选择同一订单中的1至10张电子票";
        return false;
    }
    string checkedinat = DateUtils::now();
    if (checkedinat.empty()) {
        lasterror = "无法获取当前时间";
        return false;
    }
    string today = checkedinat.substr(0, 10);
    string selectedorderid;
    vector<int> selected;
    // 检查所选门票的状态和游玩日期
    for (size_t i = 0; i < ticketids.size(); i++) {
        for (size_t j = 0; j < i; j++) {
            if (ticketids[i] == ticketids[j]) {
                lasterror = "不能重复选择同一张电子票";
                return false;
            }
        }
        int ticketindex = findticket(ticketids[i]);
        if (ticketindex == -1) {
            lasterror = "电子票不存在";
            return false;
        }
        if (tickets[ticketindex].status != TicketStatus::unused) {
            lasterror = "电子票已使用或已退款";
            return false;
        }
        int itemindex = finditem(tickets[ticketindex].itemid);
        if (itemindex == -1) {
            lasterror = "电子票缺少对应的购买明细";
            return false;
        }
        int orderindex = findorder(items[itemindex].orderid);
        if (orderindex == -1) {
            lasterror = "电子票缺少对应的订单";
            return false;
        }
        const Order& order = orders[orderindex];
        if (order.visitdate != today) {
            lasterror = "只能在门票对应的游玩日期入园";
            return false;
        }
        if (checkedinat < order.createdat) {
            lasterror = "当前时间早于订单购买时间";
            return false;
        }
        if (i == 0) {
            selectedorderid = order.orderid;
        }
        else if (order.orderid != selectedorderid) {
            lasterror = "一次只能检验同一订单中的电子票";
            return false;
        }
        selected.push_back(ticketindex);
    }
    vector<string> reserved;
    string newbatchid = generateid("KB", reserved);
    if (newbatchid.empty()) {
        lasterror = "无法生成检票批次编号";
        return false;
    }
    reserved.push_back(newbatchid);
    vector<AdmissionTicket> newtickets = tickets;
    vector<CheckInRecord> newcheckins = checkins;
    for (size_t i = 0; i < selected.size(); i++) {
        string recordid = generateid("K", reserved);
        if (recordid.empty()) {
            lasterror = "无法生成检票记录编号";
            return false;
        }
        reserved.push_back(recordid);
        int index = selected[i];
        CheckInRecord record;
        record.recordid = recordid;
        record.batchid = newbatchid;
        record.ticketid = tickets[index].ticketid;
        record.operatorid = session.getuserid();
        record.checkedinat = checkedinat;
        newcheckins.push_back(record);
        newtickets[index].status = TicketStatus::used;
    }
    tickets.swap(newtickets);
    checkins.swap(newcheckins);
    batchid = newbatchid;
    return true;
}
///状态查询
bool TicketManager::haspendingtickets(const string& visitorid,
    const string& fromdate) const {
    if (!DateUtils::validdate(fromdate)) {
        return false;
    }
    for (size_t i = 0; i < tickets.size(); i++) {
        if (tickets[i].status != TicketStatus::unused) {
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
        if (orders[orderindex].visitorid == visitorid
            && orders[orderindex].visitdate >= fromdate) {
            return true;
        }
    }
    return false;
}
bool TicketManager::hasvisited(const string& visitorid,
    const string& visitdate) const {
    if (!DateUtils::validdate(visitdate)) {
        return false;
    }
    for (size_t i = 0; i < checkins.size(); i++) {
        if (checkins[i].checkedinat.substr(0, 10) != visitdate) {
            continue;
        }
        int ticketindex = findticket(checkins[i].ticketid);
        if (ticketindex == -1) {
            continue;
        }
        int itemindex = finditem(tickets[ticketindex].itemid);
        if (itemindex == -1) {
            continue;
        }
        int orderindex = findorder(items[itemindex].orderid);
        if (orderindex == -1) {
            continue;
        }
        if (orders[orderindex].visitorid == visitorid
            && orders[orderindex].visitdate == visitdate) {
            return true;
        }
    }
    return false;
}
bool TicketManager::getdailystatistics(const Session& session,
    const string& date,
    DailyStatistics& statistics) {
    lasterror.clear();
    if (session.getrole() != Role::admin) {
        lasterror = "只有管理员可以查看运营统计";
        return false;
    }
    AvailabilityInfo availability;
    if (!getavailability(date, availability)) {
        return false;
    }
    DailyStatistics result;
    result.date = date;
    result.capacity = availability.capacity;
    result.sold = availability.sold;
    result.remaining = availability.remaining;
    // 按游玩日期统计各票种有效销量
    for (size_t i = 0; i < tickets.size(); i++) {
        if (tickets[i].status == TicketStatus::refunded) {
            continue;
        }
        int itemindex = finditem(tickets[i].itemid);
        if (itemindex == -1) {
            continue;
        }
        int orderindex = findorder(items[itemindex].orderid);
        if (orderindex == -1 || orders[orderindex].visitdate != date) {
            continue;
        }
        TicketType type = items[itemindex].type;
        if (type == TicketType::adult) {
            result.adultsold++;
        }
        else if (type == TicketType::student) {
            result.studentsold++;
        }
        else if (type == TicketType::child) {
            result.childsold++;
        }
        else {
            lasterror = "票种数据错误";
            return false;
        }
    }
    for (size_t i = 0; i < checkins.size(); i++) {
        if (checkins[i].checkedinat.substr(0, 10) == date) {
            result.checkedincount++;
        }
    }
    // 按实际发生日期统计付款和退款
    for (size_t i = 0; i < orders.size(); i++) {
        if (orders[i].createdat.substr(0, 10) == date) {
            Money amount = orders[i].totalprice;
            if (amount < 0 || amount > LLONG_MAX - result.revenue) {
                lasterror = "付款统计金额超出范围";
                return false;
            }
            result.revenue += amount;
        }
    }
    for (size_t i = 0; i < refunds.size(); i++) {
        if (refunds[i].refundedat.substr(0, 10) == date) {
            Money amount = refunds[i].amount;
            if (amount < 0 || amount > LLONG_MAX - result.refundamount) {
                lasterror = "退款统计金额超出范围";
                return false;
            }
            result.refundamount += amount;
        }
    }
    result.netrevenue = result.revenue - result.refundamount;
    statistics = result;
    return true;
}
