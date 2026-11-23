#include "visitor/visitormenu.h"
#include "visitor/visitorutils.h"
#include <iostream>
using namespace std;
char VisitorMenu::ticketmenu() {
    while (session.getrole() == Role::visitor) {
        cout << "\n========== 票务服务 ==========\n";
        cout << "当前用户：" << session.getdisplayname() << "\n";
        cout << "1. 查看票价与购票条件\n";
        cout << "2. 查询余票\n";
        cout << "3. 购买门票\n";
        cout << "4. 查询订单列表\n";
        cout << "5. 查看订单详情\n";
        cout << "6. 退还选定门票\n";
        cout << "7. 查看我的优惠券\n";
        cout << "0. 返回上一级\n";
        cout << "M. 返回系统主菜单\n";
        cout << "Q. 请求退出系统\n";
        cout << "请选择：";
        string choice;
        if (!getline(cin, choice)) {
            return 'Q';
        }
        if (choice == "0") {
            return '0';
        }
        if (choice == "M" || choice == "m") {
            return 'M';
        }
        if (choice == "Q" || choice == "q") {
            return 'Q';
        }
        if (choice == "1") {
            showprices();
        }
        else if (choice == "2") {
            showavailability();
        }
        else if (choice == "3") {
            buytickets();
        }
        else if (choice == "4") {
            queryorders();
        }
        else if (choice == "5") {
            showorderdetail();
        }
        else if (choice == "6") {
            refundtickets();
        }
        else if (choice == "7") {
            showcoupons();
        }
        else {
            cout << "无效选项，请重新输入\n";
        }
    }
    return '0';
}
void VisitorMenu::showprices() {
    vector<TicketInfo> infos = catalog.getallticketinfo();
    cout << "\n========== 票价与购票条件 ==========\n";
    for (size_t i = 0; i < infos.size(); i++) {
        const TicketInfo& info = infos[i];
        cout << info.name << "：" << moneytext(info.price) << "元";
        if (info.onsale) {
            cout << "，正在售票\n";
        }
        else {
            cout << "，已下架\n";
        }
        if (!info.eligibility.empty()) {
            cout << "适用条件：" << info.eligibility << "\n";
        }
    }
    cout << "普通游玩日期，会员可选择九折优惠，与优惠券不能叠加\n";
    cout << "节日游玩日期，所有票种八折，不叠加会员折扣或优惠券\n";
    cout << "每笔订单最多购买10张门票，游览日期必须相同\n";
    cout << "可购买今天至未来第30天的门票\n";
}
void VisitorMenu::showavailability() {
    string date;
    if (!readtext(
        "请输入游览日期，格式YYYY-MM-DD，留空查询今天", date, true)) {
        return;
    }
    if (date.empty()) {
        date = DateUtils::today();
        if (date.empty()) {
            cout << "无法获取当前日期\n";
            return;
        }
    }
    AvailabilityInfo info;
    if (!tickets.getavailability(date, info)) {
        cout << tickets.getlasterror() << "\n";
        return;
    }
    cout << "\n========== 余票查询 ==========\n";
    cout << "游览日期：" << info.visitdate << "\n";
    cout << "接待容量：" << info.capacity << "\n";
    cout << "未退款的已售门票：" << info.sold << "\n";
    cout << "剩余可售门票：" << info.remaining << "\n";
}
///购买门票的输入流程
void VisitorMenu::buytickets() {
    showprices();
    PurchaseRequest request;
    if (!readtext("请输入游览日期，格式YYYY-MM-DD",
        request.visitdate)) {
        return;
    }
    // 选择票种与数量
    TicketType types[3] = {
        TicketType::adult,
        TicketType::student,
        TicketType::child
    };
    int total = 0;
    bool needqualification = false;
    for (int i = 0; i < 3; i++) {
        if (total == 10) {
            break;
        }
        int quantity = 0;
        string prompt = "请输入" + tickettypename(types[i])
            + "数量，0表示不购买，最多"
            + to_string(10 - total) + "张";
        if (!readint(prompt, 0, 10 - total, quantity)) {
            return;
        }
        if (quantity == 0) {
            continue;
        }
        PurchaseItem item;
        item.type = types[i];
        item.quantity = quantity;
        request.items.push_back(item);
        total += quantity;
        if (types[i] != TicketType::adult) {
            needqualification = true;
        }
    }
    if (request.items.empty()) {
        cout << "未选择门票，本次购买已取消\n";
        return;
    }
    if (needqualification) {
        int answer = 0;
        if (!readint("是否符合所选学生票或儿童票的适用条件，1是、0否",
            0, 1, answer)) {
            return;
        }
        if (answer == 0) {
            cout << "不符合优惠票条件，本次购买已取消\n";
            return;
        }
        request.qualificationconfirmed = true;
    }
    // 选择优惠方式
    if (catalog.isholiday(request.visitdate)) {
        cout << "该游玩日期享受节日八折，所有票种自动优惠\n";
        cout << "本次不叠加会员折扣或优惠券\n";
    }
    else {
        cout << "\n请选择优惠方式\n";
        cout << "0. 不使用优惠\n";
        cout << "1. 会员九折，仅限会员\n";
        cout << "2. 使用一张5元优惠券\n";
        int discount = 0;
        if (!readint("请输入优惠方式", 0, 2, discount)) {
            return;
        }
        if (discount == 1) {
            request.usememberdiscount = true;
        }
        else if (discount == 2) {
            if (!readtext("请输入优惠券编号", request.couponid)) {
                return;
            }
            cout << "请选择优惠券用于哪一项中的一张门票\n";
            for (size_t i = 0; i < request.items.size(); i++) {
                cout << i + 1 << ". "
                    << tickettypename(request.items[i].type)
                    << "，共" << request.items[i].quantity << "张\n";
            }
            int selected = 0;
            if (!readint("请输入项目序号", 1,
                static_cast<int>(request.items.size()), selected)) {
                return;
            }
            request.couponitemindex = selected - 1;
        }
    }
    // 计算并显示最终金额
    PurchasePreview preview;
    if (!tickets.previewpurchase(session, request, preview)) {
        cout << tickets.getlasterror() << "\n";
        return;
    }
    cout << "\n========== 购票确认 ==========\n";
    if (preview.holidaydiscount) {
        cout << "优惠方式：节日八折\n";
    }
    else if (request.usememberdiscount) {
        cout << "优惠方式：会员九折\n";
    }
    else if (!request.couponid.empty()) {
        cout << "优惠方式：优惠券抵扣\n";
    }
    else {
        cout << "优惠方式：无\n";
    }
    cout << "游览日期：" << request.visitdate << "\n";
    for (size_t i = 0; i < request.items.size(); i++) {
        cout << tickettypename(request.items[i].type)
            << "，数量：" << request.items[i].quantity
            << "，优惠后小计："
            << moneytext(preview.itemtotals[i]) << "元\n";
    }
    cout << "总张数：" << preview.quantity << "\n";
    cout << "原价合计：" << moneytext(preview.originaltotal) << "元\n";
    cout << "优惠金额：" << moneytext(preview.discountamount) << "元\n";
    cout << "实际支付：" << moneytext(preview.totalprice) << "元\n";
    if (!confirm("是否确认购买并模拟付款")) {
        return;
    }
    string orderid;
    bool success = tickets.buytickets(session, request, orderid);
    finishoperation(session, "BUY_TICKETS", orderid,
        success, tickets.getlasterror());
}
void VisitorMenu::queryorders() {
    OrderQuery query;
    cout << "\n========== 查询订单列表 ==========\n";
    if (!readtext("购买日期，格式YYYY-MM-DD，留空不限",
        query.purchasedate, true)) {
        return;
    }
    if (!readtext("游玩日期，格式YYYY-MM-DD，留空不限",
        query.visitdate, true)) {
        return;
    }
    // 管理员可以按游客编号筛选
    if (session.getrole() == Role::admin) {
        if (!readtext("游客编号，留空查询全部游客",
            query.visitorid, true)) {
            return;
        }
    }
    cout << "电子票状态：0.不限 1.未使用 2.已使用 3.已退款\n";
    int choice;
    if (!readint("请选择状态", 0, 3, choice)) {
        return;
    }
    if (choice != 0) {
        query.filterstatus = true;
        if (choice == 1) {
            query.status = TicketStatus::unused;
        }
        else if (choice == 2) {
            query.status = TicketStatus::used;
        }
        else {
            query.status = TicketStatus::refunded;
        }
    }
    vector<OrderSummary> result;
    if (!tickets.getorders(session, query, result)) {
        cout << tickets.getlasterror() << "\n";
        return;
    }
    if (result.empty()) {
        cout << "没有符合条件的订单\n";
        return;
    }
    cout << "共找到" << result.size() << "笔订单\n";
    for (size_t i = 0; i < result.size(); i++) {
        const OrderSummary& summary = result[i];
        cout << "\n订单编号：" << summary.order.orderid << "\n";
        cout << "游客编号：" << summary.order.visitorid << "\n";
        cout << "购买时间：" << summary.order.createdat << "\n";
        cout << "游玩日期：" << summary.order.visitdate << "\n";
        cout << "购买总数：" << summary.totalquantity << "张\n";
        cout << "未使用：" << summary.unusedquantity << "张";
        cout << "，已使用：" << summary.usedquantity << "张";
        cout << "，已退款：" << summary.refundedquantity << "张\n";
        cout << "原实付金额："
            << moneytext(summary.order.totalprice) << "元\n";
        cout << "已退款金额："
            << moneytext(summary.refundedamount) << "元\n";
    }
}
///查看订单详情
void VisitorMenu::showorderdetail() {
    string orderid;
    if (!readtext("请输入订单编号", orderid)) {
        return;
    }
    OrderDetail detail;
    if (!tickets.getorderdetail(session, orderid, detail)) {
        cout << tickets.getlasterror() << "\n";
        return;
    }
    showorder(detail);
}
void VisitorMenu::showorder(const OrderDetail& detail) const {
    cout << "\n========== 订单详情 ==========\n";
    cout << "订单编号：" << detail.order.orderid << "\n";
    cout << "游客编号：" << detail.order.visitorid << "\n";
    cout << "购买时间：" << detail.order.createdat << "\n";
    cout << "游玩日期：" << detail.order.visitdate << "\n";
    cout << "原实付金额："
        << moneytext(detail.order.totalprice) << "元\n";
    cout << "会员折扣：";
    if (detail.order.memberdiscount) {
        cout << "已使用\n";
    }
    else {
        cout << "未使用\n";
    }
    cout << "节日优惠：";
    if (detail.order.holidaydiscount) {
        cout << "八折\n";
    }
    else {
        cout << "未使用\n";
    }
    // 按购买明细分别显示各票种的电子票
    for (size_t i = 0; i < detail.items.size(); i++) {
        const OrderItem& item = detail.items[i];
        cout << "\n票种：" << tickettypename(item.type) << "\n";
        cout << "购买数量：" << item.quantity << "张\n";
        cout << "购买时基础单价："
            << moneytext(item.unitprice) << "元\n";
        for (size_t j = 0; j < detail.tickets.size(); j++) {
            const AdmissionTicket& ticket = detail.tickets[j];
            if (ticket.itemid != item.itemid) {
                continue;
            }
            cout << "票号：" << ticket.ticketid;
            cout << "，实付：" << moneytext(ticket.paidprice) << "元";
            cout << "，状态：" << ticketstatusname(ticket.status) << "\n";
            if (!ticket.couponid.empty()) {
                cout << "使用的优惠券：" << ticket.couponid << "\n";
            }
        }
    }
    cout << "\n========== 退款记录 ==========\n";
    if (detail.refunds.empty()) {
        cout << "暂无退款记录\n";
    }
    for (size_t i = 0; i < detail.refunds.size(); i++) {
        const RefundRecord& record = detail.refunds[i];
        cout << "票号：" << record.ticketid << "\n";
        cout << "退款批次：" << record.batchid << "\n";
        cout << "退款金额：" << moneytext(record.amount) << "元\n";
        cout << "退款时间：" << record.refundedat << "\n";
    }
    cout << "\n========== 检票记录 ==========\n";
    if (detail.checkins.empty()) {
        cout << "暂无检票记录\n";
    }
    for (size_t i = 0; i < detail.checkins.size(); i++) {
        const CheckInRecord& record = detail.checkins[i];
        cout << "票号：" << record.ticketid << "\n";
        cout << "检票批次：" << record.batchid << "\n";
        cout << "操作人：" << record.operatorid << "\n";
        cout << "检票时间：" << record.checkedinat << "\n";
    }
}
///退还选中的电子票
void VisitorMenu::refundtickets() {
    cout << "\n========== 电子票退款 ==========\n";
    cout << "请先通过订单详情查看需要退款的票号\n";
    cout << "只能退还本人未使用且游玩日期晚于今天的电子票\n";
    cout << "按每张票的实际付款金额退款，已使用的优惠券不退回\n";
    cout << "可以只退订单中的部分电子票\n";
    vector<string> ticketids;
    if (!readticketids(
        "请输入需要退款的票号，最多10张",
        ticketids)) {
        return;
    }
    cout << "\n本次申请退款的电子票：\n";
    for (size_t i = 0; i < ticketids.size(); i++) {
        cout << i + 1 << ". " << ticketids[i] << "\n";
    }
    cout << "所选电子票全部符合条件时才会执行本次退款\n";
    if (!confirm("是否确认退还以上电子票")) {
        return;
    }
    const Session actor = session;
    string batchid;
    bool success = tickets.refundtickets(session, ticketids, batchid);
    finishoperation(actor, "REFUND_TICKETS", batchid,
        success, tickets.getlasterror());
}
///查看我的优惠券
void VisitorMenu::showcoupons() {
    vector<Coupon> result;
    if (!coupons.getcoupons(session, result)) {
        cout << coupons.getlasterror() << "\n";
        return;
    }
    cout << "\n========== 优惠券列表 ==========\n";
    if (result.empty()) {
        cout << "暂无优惠券\n";
        return;
    }
    string today = DateUtils::today();
    cout << "每笔订单最多使用一张优惠券，不能与会员折扣叠加\n";
    cout << "有效期包含截止日期当天，退款不退回优惠券\n";
    for (size_t i = 0; i < result.size(); i++) {
        const Coupon& coupon = result[i];
        cout << "\n优惠券编号：" << coupon.couponid << "\n";
        cout << "抵扣金额：" << moneytext(coupon.amount) << "元\n";
        cout << "发放时间：" << coupon.issuedat << "\n";
        cout << "有效期至：" << coupon.expirydate << "\n";
        cout << "状态：";
        if (coupon.status == CouponStatus::used) {
            cout << "已使用\n";
            cout << "对应票号：" << coupon.ticketid << "\n";
            cout << "使用时间：" << coupon.usedat << "\n";
        }
        else if (today.empty()) {
            cout << "未使用，无法获取当前日期以判断是否过期\n";
        }
        else if (coupon.expirydate < today) {
            cout << "已过期\n";
        }
        else {
            cout << "未使用，在有效期内\n";
        }
    }
}
