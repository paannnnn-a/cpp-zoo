#include "visitor/cstorage.h"
#include "visitor/visitormanager.h"
#include "visitor/ticketcatalog.h"
#include "visitor/couponmanager.h"
#include "visitor/ticketmanager.h"
#include "visitor/feedbackmanager.h"
#include <utility>
using namespace std;

CStorage::CStorage(const string& datadirectory,
    VisitorManager& visitormanager,
    TicketCatalog& ticketcatalog,
    CouponManager& couponmanager,
    TicketManager& ticketmanager,
    FeedbackManager& feedbackmanager)
    : directory(datadirectory), visitors(visitormanager),
    catalog(ticketcatalog), coupons(couponmanager),
    tickets(ticketmanager), feedback(feedbackmanager) {
}
bool CStorage::checklinks(VisitorManager& loadedvisitors,
    CouponManager& loadedcoupons,
    TicketManager& loadedtickets) {
    for (size_t i = 0; i < loadedcoupons.coupons.size(); i++) {
        const Coupon& coupon = loadedcoupons.coupons[i];
        if (loadedvisitors.findvisitor(coupon.visitorid) == -1) {
            lasterror = "优惠券 " + coupon.couponid + " 对应的游客不存在";
            return false;
        }
        if (coupon.status == CouponStatus::unused) {
            continue;
        }
        int index = loadedtickets.findticket(coupon.ticketid);
        if (index == -1
            || loadedtickets.tickets[index].couponid != coupon.couponid) {
            lasterror = "优惠券 " + coupon.couponid
                + " 对应的电子票不存在或关联不一致";
            return false;
        }
    }
    return true;
}
bool CStorage::save() {
    lasterror.clear();
    if (directory.empty()) {
        lasterror = "数据目录不能为空";
        return false;
    }
    if (!checklinks(visitors, coupons, tickets)) {
        return false;
    }
    string prefix = directory;
    if (prefix.back() != '/' && prefix.back() != '\\') {
        prefix += "/";
    }
    if (!visitors.savetofile(prefix + "visitors.txt")) {
        lasterror = "游客资料保存失败：" + visitors.getlasterror();
        return false;
    }
    if (!catalog.savetofile(prefix + "ticket_types.txt")) {
        lasterror = "票种配置保存失败：" + catalog.getlasterror();
        return false;
    }
    if (!catalog.saveholidaystofile(prefix + "holiday_settings.txt")) {
        lasterror = "节日规则保存失败：" + catalog.getlasterror();
        return false;
    }
    if (!coupons.savetofile(prefix + "coupons.txt")) {
        lasterror = "优惠券保存失败：" + coupons.getlasterror();
        return false;
    }
    if (!tickets.savetofiles(directory)) {
        lasterror = "票务数据保存失败：" + tickets.getlasterror();
        return false;
    }
    if (!feedback.savetofile(prefix + "feedback.txt")) {
        lasterror = "反馈保存失败：" + feedback.getlasterror();
        return false;
    }
    return true;
}
bool CStorage::load() {
    lasterror.clear();
    if (directory.empty()) {
        lasterror = "数据目录不能为空";
        return false;
    }
    string prefix = directory;
    if (prefix.back() != '/' && prefix.back() != '\\') {
        prefix += "/";
    }
    // 创建相互关联的临时管理器
    VisitorManager loadedvisitors;
    TicketCatalog loadedcatalog;
    CouponManager loadedcoupons(loadedvisitors);
    TicketManager loadedtickets(loadedvisitors, loadedcatalog, loadedcoupons);
    FeedbackManager loadedfeedback(loadedvisitors, loadedtickets);
    if (!loadedvisitors.loadfromfile(prefix + "visitors.txt")) {
        lasterror = "游客资料加载失败：" + loadedvisitors.getlasterror();
        return false;
    }
    if (!loadedcatalog.loadfromfile(prefix + "ticket_types.txt")) {
        lasterror = "票种配置加载失败：" + loadedcatalog.getlasterror();
        return false;
    }
    if (!loadedcatalog.loadholidaysfromfile(prefix + "holiday_settings.txt")) {
        lasterror = "节日规则加载失败：" + loadedcatalog.getlasterror();
        return false;
    }
    if (!loadedcoupons.loadfromfile(prefix + "coupons.txt")) {
        lasterror = "优惠券加载失败：" + loadedcoupons.getlasterror();
        return false;
    }
    if (!loadedtickets.loadfromfiles(directory)) {
        lasterror = "票务数据加载失败：" + loadedtickets.getlasterror();
        return false;
    }
    if (!loadedfeedback.loadfromfile(prefix + "feedback.txt")) {
        lasterror = "反馈加载失败：" + loadedfeedback.getlasterror();
        return false;
    }
    if (!checklinks(loadedvisitors, loadedcoupons, loadedtickets)) {
        return false;
    }
    // 全部检查通过后更新正式数据
    visitors.visitors.swap(loadedvisitors.visitors);
    swap(catalog.adult, loadedcatalog.adult);
    swap(catalog.student, loadedcatalog.student);
    swap(catalog.child, loadedcatalog.child);
    catalog.holidays.swap(loadedcatalog.holidays);
    coupons.coupons.swap(loadedcoupons.coupons);
    tickets.orders.swap(loadedtickets.orders);
    tickets.items.swap(loadedtickets.items);
    tickets.tickets.swap(loadedtickets.tickets);
    tickets.refunds.swap(loadedtickets.refunds);
    tickets.checkins.swap(loadedtickets.checkins);
    tickets.capacities.swap(loadedtickets.capacities);
    tickets.defaultcapacity = loadedtickets.defaultcapacity;
    feedback.feedbacks.swap(loadedfeedback.feedbacks);
    visitors.lasterror.clear();
    catalog.lasterror.clear();
    coupons.lasterror.clear();
    tickets.lasterror.clear();
    feedback.lasterror.clear();
    return true;
}
string CStorage::getlasterror() const {
    return lasterror;
}
