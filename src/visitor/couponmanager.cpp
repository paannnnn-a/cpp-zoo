#include "visitor/couponmanager.h"
#include "visitor/couponmanager.h"
#include "visitor/visitorutils.h"
#include <sstream>
#include<fstream>
#include <iomanip>
using namespace std;

///优惠券查找和编号生成
CouponManager::CouponManager(VisitorManager& manager)
    : visitors(manager) {
}
int CouponManager::findcoupon(const string& couponid) const {
    for (size_t i = 0; i < coupons.size(); i++) {
        if (coupons[i].couponid == couponid) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
string CouponManager::generateid() const {
    for (int number = 1; number <= 99999999; number++) {
        ostringstream result;
        result << "C" << setfill('0') << setw(4) << number;
        string id = result.str();
        if (findcoupon(id) == -1) {
            return id;
        }
    }
    return "";
}
///实现管理员发券
bool CouponManager::issuecoupon(const Session& session,
    const string& visitorid,
    const string& expirydate,
    string& couponid) {
    lasterror.clear();
    if (session.getrole() != Role::admin) {
        lasterror = "只有管理员可以发放优惠券";
        return false;
    }
    Visitor visitor;
    if (!visitors.getvisitorinfo(session, visitorid, visitor)) {
        lasterror = visitors.getlasterror();
        return false;
    }
    if (!visitor.getactive()) {
        lasterror = "不能向已注销账号发放优惠券";
        return false;
    }
    if (!DateUtils::validdate(expirydate)) {
        lasterror = "有效期必须是合法的YYYY-MM-DD日期";
        return false;
    }
    string issuedat = DateUtils::now();
    if (issuedat.empty()) {
        lasterror = "无法获取当前时间";
        return false;
    }
    string today = issuedat.substr(0, 10);
    if (expirydate < today) {
        lasterror = "有效期不能早于今天";
        return false;
    }
    string id = generateid();
    if (id.empty()) {
        lasterror = "无法生成优惠券编号";
        return false;
    }
    Coupon coupon;
    coupon.couponid = id;
    coupon.visitorid = visitorid;
    coupon.expirydate = expirydate;
    coupon.issuedat = issuedat;
    coupons.push_back(coupon);
    couponid = id;
    return true;
}
///实现优惠券列表查询
bool CouponManager::getcoupons(const Session& session,
    vector<Coupon>& result) {
    lasterror.clear();
    Role role = session.getrole();
    if (role != Role::visitor && role != Role::admin) {
        lasterror = "请先登录";
        return false;
    }
    if (role == Role::visitor) {
        Visitor visitor;
        if (!visitors.getvisitorinfo(session, session.getuserid(), visitor)) {
            lasterror = visitors.getlasterror();
            return false;
        }
    }
    vector<Coupon> found;
    for (size_t i = 0; i < coupons.size(); i++) {
        if (role == Role::admin
            || coupons[i].visitorid == session.getuserid()) {
            found.push_back(coupons[i]);
        }
    }
    result = found;
    return true;
}
///单张优惠券查询
bool CouponManager::getcouponinfo(const Session& session,
    const string& couponid,
    Coupon& coupon) {
    lasterror.clear();
    Role role = session.getrole();
    if (role != Role::visitor && role != Role::admin) {
        lasterror = "请先登录";
        return false;
    }
    if (role == Role::visitor) {
        Visitor visitor;
        if (!visitors.getvisitorinfo(session, session.getuserid(), visitor)) {
            lasterror = visitors.getlasterror();
            return false;
        }
    }
    int index = findcoupon(couponid);
    if (index == -1) {
        lasterror = "优惠券不存在";
        return false;
    }
    if (role == Role::visitor
        && coupons[index].visitorid != session.getuserid()) {
        lasterror = "只能查询本人的优惠券";
        return false;
    }
    coupon = coupons[index];
    return true;
}
///检查优惠券能否用来购票
bool CouponManager::checkcoupon(const Session& session,
    const string& couponid,
    Money ticketprice,
    Coupon& coupon) {
    lasterror.clear();
    if (session.getrole() != Role::visitor) {
        lasterror = "请使用游客账号购票";
        return false;
    }
    Coupon result;
    if (!getcouponinfo(session, couponid, result)) {
        return false;
    }
    if (result.status != CouponStatus::unused) {
        lasterror = "优惠券已经使用";
        return false;
    }
    string today = DateUtils::today();
    if (today.empty()) {
        lasterror = "无法获取当前日期";
        return false;
    }
    if (result.expirydate < today) {
        lasterror = "优惠券已过期";
        return false;
    }
    if (ticketprice < result.amount) {
        lasterror = "单张票金额不能低于优惠券抵扣金额";
        return false;
    }
    coupon = result;
    return true;
}
///记录优惠券使用
bool CouponManager::usecoupon(const Session& session,
    const string& couponid,
    const string& ticketid) {
    lasterror.clear();
    if (session.getrole() != Role::visitor) {
        lasterror = "请使用游客账号购票";
        return false;
    }
    Coupon coupon;
    if (!getcouponinfo(session, couponid, coupon)) {
        return false;
    }
    if (coupon.status != CouponStatus::unused) {
        lasterror = "优惠券已经使用";
        return false;
    }
    if (ticketid == "-" || !FieldValidation::validid(ticketid)) {
        lasterror = "电子票编号格式错误";
        return false;
    }
    string usedat = DateUtils::now();
    if (usedat.empty()) {
        lasterror = "无法获取当前时间";
        return false;
    }
    if (coupon.expirydate < usedat.substr(0, 10)) {
        lasterror = "优惠券已过期";
        return false;
    }
    for (size_t i = 0; i < coupons.size(); i++) {
        if (coupons[i].status == CouponStatus::used
            && coupons[i].ticketid == ticketid) {
            lasterror = "该电子票已经使用了优惠券";
            return false;
        }
    }
    int index = findcoupon(couponid);
    coupons[index].status = CouponStatus::used;
    coupons[index].ticketid = ticketid;
    coupons[index].usedat = usedat;
    return true;
}
///保存
bool CouponManager::savetofile(const string& filename) {
    lasterror.clear();
    for (size_t i = 0; i < coupons.size(); i++) {
        const Coupon& coupon = coupons[i];
        if (!FieldValidation::validid(coupon.couponid)
            || !FieldValidation::validid(coupon.visitorid)
            || coupon.amount != 500
            || !DateUtils::validdate(coupon.expirydate)
            || !DateUtils::validdatetime(coupon.issuedat)) {
            lasterror = "优惠券资料格式错误，无法保存";
            return false;
        }
        if (coupon.expirydate < coupon.issuedat.substr(0, 10)) {
            lasterror = "优惠券有效期早于发放日期";
            return false;
        }
        if (coupon.status == CouponStatus::unused) {
            if (!coupon.ticketid.empty() || !coupon.usedat.empty()) {
                lasterror = "未使用的优惠券不应有使用记录";
                return false;
            }
        }
        else if (coupon.status == CouponStatus::used) {
            if (coupon.ticketid == "-" || !FieldValidation::validid(coupon.ticketid)
                || !DateUtils::validdatetime(coupon.usedat)) {
                lasterror = "已使用优惠券的使用记录格式错误";
                return false;
            }
            if (coupon.usedat < coupon.issuedat
                || coupon.usedat.substr(0, 10) > coupon.expirydate) {
                lasterror = "优惠券使用时间不符合有效期";
                return false;
            }
        }
        else {
            lasterror = "优惠券状态错误";
            return false;
        }
    }
    ofstream file(filename);
    if (!file.is_open()) {
        lasterror = "无法打开优惠券文件";
        return false;
    }
    for (size_t i = 0; i < coupons.size(); i++) {
        const Coupon& coupon = coupons[i];
        string status = "UNUSED";
        string ticketid = "-";
        string usedat = "-";
        if (coupon.status == CouponStatus::used) {
            status = "USED";
            ticketid = coupon.ticketid;
            usedat = coupon.usedat;
        }
        file << coupon.couponid << "|"
            << coupon.visitorid << "|"
            << coupon.amount << "|"
            << coupon.expirydate << "|"
            << coupon.issuedat << "|"
            << status << "|"
            << ticketid << "|"
            << usedat << "\n";
    }
    file.close();
    if (file.fail()) {
        lasterror = "优惠券文件写入失败";
        return false;
    }
    return true;
}
///读取优惠券文件
bool CouponManager::loadfromfile(const string& filename) {
    lasterror.clear();
    ifstream file(filename);
    if (!file.is_open()) {
        lasterror = "无法打开优惠券文件";
        return false;
    }
    vector<Coupon> loaded;
    string line;
    int linenumber = 0;
    while (getline(file, line)) {
        linenumber++;
        if (linenumber == 1 && line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }
        vector<string> fields;
        string field;
        stringstream row(line);
        while (getline(row, field, '|')) {
            fields.push_back(field);
        }
        string position = "第" + to_string(linenumber) + "行";
        if (fields.size() != 8 || line.back() == '|') {
            lasterror = position + "字段数量错误";
            return false;
        }
        if (!FieldValidation::validid(fields[0])
            || !FieldValidation::validid(fields[1])) {
            lasterror = position + "优惠券编号或游客编号格式错误";
            return false;
        }
        if (fields[2] != "500") {
            lasterror = position + "优惠券金额必须为500分";
            return false;
        }
        if (!DateUtils::validdate(fields[3])
            || !DateUtils::validdatetime(fields[4])) {
            lasterror = position + "有效期或发放时间格式错误";
            return false;
        }
        if (fields[3] < fields[4].substr(0, 10)) {
            lasterror = position + "有效期早于发放日期";
            return false;
        }
        Coupon coupon;
        coupon.couponid = fields[0];
        coupon.visitorid = fields[1];
        coupon.expirydate = fields[3];
        coupon.issuedat = fields[4];
        if (fields[5] == "UNUSED") {
            if (fields[6] != "-" || fields[7] != "-") {
                lasterror = position + "未使用的优惠券不应有使用记录";
                return false;
            }
        }
        else if (fields[5] == "USED") {
            if (fields[6] == "-"
                || !FieldValidation::validid(fields[6])
                || !DateUtils::validdatetime(fields[7])) {
                lasterror = position + "使用记录格式错误";
                return false;
            }
            if (fields[7] < fields[4]
                || fields[7].substr(0, 10) > fields[3]) {
                lasterror = position + "使用时间不符合有效期";
                return false;
            }
            coupon.status = CouponStatus::used;
            coupon.ticketid = fields[6];
            coupon.usedat = fields[7];
        }
        else {
            lasterror = position + "优惠券状态错误";
            return false;
        }
        for (size_t i = 0; i < loaded.size(); i++) {
            if (loaded[i].couponid == coupon.couponid) {
                lasterror = position + "优惠券编号重复";
                return false;
            }
            if (coupon.status == CouponStatus::used
                && loaded[i].status == CouponStatus::used
                && loaded[i].ticketid == coupon.ticketid) {
                lasterror = position + "同一张电子票使用了多张优惠券";
                return false;
            }
        }
        loaded.push_back(coupon);
    }
    if (file.bad() || !file.eof()) {
        lasterror = "读取优惠券文件失败";
        return false;
    }
    coupons = loaded;
    return true;
}
string CouponManager::getlasterror() const {
    return lasterror;
}