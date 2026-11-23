#include "visitor/visitormenu.h"
#include "visitor/visitorutils.h"
#include <iostream>
#include <climits>
using namespace std;
char VisitorMenu::adminmenu() {
    while (session.getrole() == Role::admin) {
        cout << "\n========== 游客与票务管理 ==========\n";
        cout << "1. 查看票种资料\n";
        cout << "2. 修改票价\n";
        cout << "3. 设置票种上架或下架\n";
        cout << "4. 查询每日余票\n";
        cout << "5. 设置每日接待容量\n";
        cout << "6. 查询订单列表\n";
        cout << "7. 查看订单详情\n";
        cout << "8. 检票入园\n";
        cout << "9. 查看每日统计\n";
        cout << "10. 设置游客会员资格\n";
        cout << "11. 发放优惠券\n";
        cout << "12. 查询反馈列表\n";
        cout << "13. 查看反馈详情\n";
        cout << "14. 处理反馈\n";
        cout << "15. 节日优惠管理\n";
        cout << "0. 返回上一级\n";
        cout << "M. 返回系统主菜单\n";
        cout << "Q. 保存并退出系统\n";
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
            updateprice();
        }
        else if (choice == "3") {
            setonsale();
        }
        else if (choice == "4") {
            showavailability();
        }
        else if (choice == "5") {
            setdailycapacity();
        }
        else if (choice == "6") {
            queryorders();
        }
        else if (choice == "7") {
            showorderdetail();
        }
        else if (choice == "8") {
            checkintickets();
        }
        else if (choice == "9") {
            showstatistics();
        }
        else if (choice == "10") {
            setmembership();
        }
        else if (choice == "11") {
            issuecoupon();
        }
        else if (choice == "12") {
            queryfeedback();
        }
        else if (choice == "13") {
            showfeedbackdetail();
        }
        else if (choice == "14") {
            handlefeedback();
        }
        else if (choice == "15") {
            char navigation = holidaymenu();
            if (navigation == 'M' || navigation == 'Q') {
                return navigation;
            }
        }
        else {
            cout << "无效选项，请重新输入\n";
        }
    }
    return '0';
}
void VisitorMenu::updateprice() {
    cout << "\n========== 修改票价 ==========\n";
    showprices();
    cout << "票种：1.成人票 2.学生票 3.儿童票\n";
    int choice;
    if (!readint("请选择票种", 1, 3, choice)) {
        return;
    }
    TicketType types[3] = {
        TicketType::adult,
        TicketType::student,
        TicketType::child
    };
    TicketType type = types[choice - 1];
    Money price;
    if (!readmoney("请输入新票价，单位为元，最多两位小数", price)) {
        return;
    }
    bool success = catalog.updateprice(session, type, price);
    finishoperation(session, "UPDATE_PRICE", tickettypename(type),
        success, catalog.getlasterror());
}
void VisitorMenu::setonsale() {
    cout << "\n========== 设置售票状态 ==========\n";
    showprices();
    cout << "票种：1.成人票 2.学生票 3.儿童票\n";
    int choice;
    if (!readint("请选择票种", 1, 3, choice)) {
        return;
    }
    TicketType types[3] = {
        TicketType::adult,
        TicketType::student,
        TicketType::child
    };
    TicketType type = types[choice - 1];
    int state;
    if (!readint("请选择售票状态，1上架，0下架", 0, 1, state)) {
        return;
    }
    bool onsale = (state == 1);
    bool success = catalog.setonsale(session, type, onsale);
    finishoperation(session, "SET_ONSALE", tickettypename(type),
        success, catalog.getlasterror());
}
void VisitorMenu::setdailycapacity() {
    cout << "\n========== 设置每日接待容量 ==========\n";
    string date;
    if (!readtext("请输入日期，格式YYYY-MM-DD", date)) {
        return;
    }
    AvailabilityInfo info;
    if (!tickets.getavailability(date, info)) {
        cout << tickets.getlasterror() << "\n";
        return;
    }
    cout << "当前接待容量：" << info.capacity << "\n";
    cout << "未退款的已售票数：" << info.sold << "\n";
    cout << "当前剩余可售票数：" << info.remaining << "\n";
    cout << "新容量不能少于" << info.sold << "\n";
    int capacity;
    if (!readint("请输入新容量", info.sold, INT_MAX, capacity)) {
        return;
    }
    bool success = tickets.setdailycapacity(session, date, capacity);
    finishoperation(session, "SET_DAILY_CAPACITY", date,
        success, tickets.getlasterror());
}
void VisitorMenu::checkintickets() {
    cout << "\n========== 检票入园 ==========\n";
    cout << "一次可检验同一订单中的1至10张电子票\n";
    cout << "仅限游玩日期为今天、尚未使用且未退款的电子票\n";
    cout << "学生票和儿童票请先核验对应的优惠资格\n";
    cout << "所选电子票全部符合条件时才会执行本次检票\n";
    vector<string> ticketids;
    if (!readticketids(
        "请输入票号，直接回车取消",
        ticketids)) {
        return;
    }
    string batchid;
    bool success = tickets.checkintickets(session, ticketids, batchid);
    finishoperation(session, "CHECK_IN", batchid,
        success, tickets.getlasterror());
}
///查看每日统计
void VisitorMenu::showstatistics() {
    cout << "\n========== 每日统计 ==========\n";
    string date;
    if (!readtext("请输入统计日期，格式YYYY-MM-DD，留空查询今天",
        date, true)) {
        return;
    }
    if (date.empty()) {
        date = DateUtils::today();
        if (date.empty()) {
            cout << "无法获取当前日期\n";
            return;
        }
    }
    DailyStatistics info;
    if (!tickets.getdailystatistics(session, date, info)) {
        cout << tickets.getlasterror() << "\n";
        return;
    }
    cout << "\n统计日期：" << info.date << "\n";
    cout << "\n【该游玩日期的售票情况】\n";
    cout << "接待容量：" << info.capacity << "\n";
    cout << "未退款的已售票数：" << info.sold << "张\n";
    cout << "成人票：" << info.adultsold << "张\n";
    cout << "学生票：" << info.studentsold << "张\n";
    cout << "儿童票：" << info.childsold << "张\n";
    cout << "剩余可售票数：" << info.remaining << "张\n";
    cout << "\n【当天实际检票情况】\n";
    cout << "检票入园票数：" << info.checkedincount << "张\n";
    cout << "\n【当天发生的收退款】\n";
    cout << "收款金额：" << moneytext(info.revenue) << "元\n";
    cout << "退款金额：" << moneytext(info.refundamount) << "元\n";
    cout << "收款减退款：" << moneytext(info.netrevenue) << "元\n";
}
///设置游客会员资格
void VisitorMenu::setmembership() {
    cout << "\n========== 设置游客会员资格 ==========\n";
    string visitorid;
    if (!readtext("请输入游客编号", visitorid)) {
        return;
    }
    Visitor visitor;
    if (!visitors.getvisitorinfo(session, visitorid, visitor)) {
        cout << visitors.getlasterror() << "\n";
        return;
    }
    cout << "游客编号：" << visitor.getvisitorid() << "\n";
    cout << "用户名：" << visitor.getusername() << "\n";
    cout << "昵称：" << visitor.getnickname() << "\n";
    if (!visitor.getactive()) {
        cout << "该游客账号已注销，不能设置会员资格\n";
        return;
    }
    cout << "当前会员资格：";
    if (visitor.getmember()) {
        cout << "会员\n";
    }
    else {
        cout << "非会员\n";
    }
    int state;
    if (!readint("请选择新资格，1会员，0非会员", 0, 1, state)) {
        return;
    }
    bool member = (state == 1);
    bool success = visitors.setmembership(session, visitorid, member);
    finishoperation(session, "SET_MEMBERSHIP", visitorid,
        success, visitors.getlasterror());
}
///发放优惠券
void VisitorMenu::issuecoupon() {
    cout << "\n========== 发放优惠券 ==========\n";
    cout << "每次向指定游客发放一张5元优惠券\n";
    string visitorid;
    if (!readtext("请输入游客编号", visitorid)) {
        return;
    }
    Visitor visitor;
    if (!visitors.getvisitorinfo(session, visitorid, visitor)) {
        cout << visitors.getlasterror() << "\n";
        return;
    }
    if (!visitor.getactive()) {
        cout << "该游客账号已注销，不能发放优惠券\n";
        return;
    }
    cout << "用户名：" << visitor.getusername() << "\n";
    cout << "昵称：" << visitor.getnickname() << "\n";
    cout << "有效期包含截止日期当天，截止日期不能早于今天\n";
    string expirydate;
    if (!readtext("请输入有效期截止日期，格式YYYY-MM-DD",
        expirydate)) {
        return;
    }
    string couponid;
    bool success = coupons.issuecoupon(
        session, visitorid, expirydate, couponid);
    finishoperation(session, "ISSUE_COUPON", couponid,
        success, coupons.getlasterror());
}
///处理反馈
void VisitorMenu::handlefeedback() {
    cout << "\n========== 处理反馈 ==========\n";
    string feedbackid;
    if (!readtext("请输入反馈编号", feedbackid)) {
        return;
    }
    Feedback item;
    if (!feedback.getfeedbackdetail(session, feedbackid, item)) {
        cout << feedback.getlasterror() << "\n";
        return;
    }
    showfeedback(item);
    cout << "\n处理状态：1.处理中 2.已解决\n";
    cout << "已解决的反馈不能退回处理中\n";
    int choice;
    if (!readint("请选择新的处理状态", 1, 2, choice)) {
        return;
    }
    FeedbackStatus status = FeedbackStatus::processing;
    if (choice == 2) {
        status = FeedbackStatus::resolved;
    }
    cout << "回复请写在一行内，不要包含竖线|\n";
    string reply;
    if (status == FeedbackStatus::processing) {
        if (!readtext("请输入回复，留空表示本次不填写回复",
            reply, true)) {
            return;
        }
    }
    else {
        if (!readtext("请输入处理结果回复", reply)) {
            return;
        }
    }
    bool success = feedback.handlefeedback(
        session, feedbackid, status, reply);
    finishoperation(session, "HANDLE_FEEDBACK", feedbackid,
        success, feedback.getlasterror());
}
char VisitorMenu::holidaymenu() {
    while (session.getrole() == Role::admin) {
        cout << "\n========== 节日优惠管理 ==========\n";
        cout << "节日按游玩日期生效，所有票种八折，不叠加会员或优惠券\n";
        cout << "添加时年份填0，表示每年在同一月日生效\n";
        cout << "1. 查看节日规则\n";
        cout << "2. 添加节日规则\n";
        cout << "3. 删除节日规则\n";
        cout << "0. 返回上一级\n";
        cout << "M. 返回系统主菜单\n";
        cout << "Q. 保存并退出系统\n";
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
            showholidays();
        }
        else if (choice == "2") {
            addholiday();
        }
        else if (choice == "3") {
            removeholiday();
        }
        else {
            cout << "无效选项，请重新输入\n";
        }
    }
    return '0';
}
void VisitorMenu::showholidays() {
    vector<HolidayRule> rules;
    if (!catalog.getholidays(session, rules)) {
        cout << catalog.getlasterror() << "\n";
        return;
    }
    cout << "\n========== 节日规则列表 ==========\n";
    if (rules.empty()) {
        cout << "暂无节日规则\n";
        return;
    }
    for (size_t i = 0; i < rules.size(); i++) {
        cout << i + 1 << ". ";
        if (rules[i].year == 0) {
            cout << "每年";
        }
        else {
            cout << rules[i].year << "年";
        }
        cout << rules[i].month << "月" << rules[i].day << "日，所有票种八折\n";
    }
}
void VisitorMenu::addholiday() {
    cout << "\n========== 添加节日规则 ==========\n";
    cout << "分别输入年月日，月份和日期可以直接输入数字，不必补0\n";
    int year;
    int month;
    int day;
    if (!readint("请输入年份，0表示每年，其他年份为1至9999", 0, 9999, year)) {
        return;
    }
    if (!readint("请输入月份", 1, 12, month)) {
        return;
    }
    if (!readint("请输入日期", 1, 31, day)) {
        return;
    }
    bool success = catalog.addholiday(session, year, month, day);
    string target = to_string(year) + "-" + to_string(month) + "-" + to_string(day);
    finishoperation(session, "ADD_HOLIDAY", target, success, catalog.getlasterror());
}
void VisitorMenu::removeholiday() {
    vector<HolidayRule> rules;
    if (!catalog.getholidays(session, rules)) {
        cout << catalog.getlasterror() << "\n";
        return;
    }
    showholidays();
    if (rules.empty()) {
        return;
    }
    int number;
    if (!readint("请输入要删除的规则序号", 1,
        static_cast<int>(rules.size()), number)) {
        return;
    }
    const HolidayRule& rule = rules[number - 1];
    bool success = catalog.removeholiday(session, rule.year, rule.month, rule.day);
    string target = to_string(rule.year) + "-" + to_string(rule.month) + "-" + to_string(rule.day);
    finishoperation(session, "REMOVE_HOLIDAY", target, success, catalog.getlasterror());
}
