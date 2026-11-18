#include "visitor/ticketmanager.h"
#include "visitor/visitorutils.h"
#include <fstream>
#include <sstream>
#include <climits>
using namespace std;
///票务文件保存
static string tickettypecode(TicketType type) {
    if (type == TicketType::adult) {
        return "adult";
    }
    if (type == TicketType::student) {
        return "student";
    }
    if (type == TicketType::child) {
        return "child";
    }
    return "";
}
static string ticketstatuscode(TicketStatus status) {
    if (status == TicketStatus::unused) {
        return "UNUSED";
    }
    if (status == TicketStatus::used) {
        return "USED";
    }
    if (status == TicketStatus::refunded) {
        return "REFUNDED";
    }
    return "";
}
bool TicketManager::savetofiles(const string& directory) {
    lasterror.clear();
    if (directory.empty()) {
        lasterror = "保存目录不能为空";
        return false;
    }
    for (size_t i = 0; i < items.size(); i++) {
        if (tickettypecode(items[i].type).empty()) {
            lasterror = "购买明细中的票种错误";
            return false;
        }
    }
    for (size_t i = 0; i < tickets.size(); i++) {
        if (ticketstatuscode(tickets[i].status).empty()) {
            lasterror = "电子票状态错误";
            return false;
        }
    }
    string prefix = directory;
    if (prefix.back() != '/' && prefix.back() != '\\') {
        prefix += "/";
    }
    string filenames[6] = {
        "orders.txt",
        "order_items.txt",
        "tickets.txt",
        "refund_records.txt",
        "checkin_records.txt",
        "capacity_settings.txt"
    };
    ofstream files[6];
    for (int i = 0; i < 6; i++) {
        files[i].open(prefix + filenames[i]);
        if (!files[i].is_open()) {
            lasterror = "无法打开文件：" + filenames[i];
            return false;
        }
    }
    for (size_t i = 0; i < orders.size(); i++) {
        const Order& order = orders[i];
        files[0] << order.orderid << "|"
            << order.visitorid << "|"
            << order.visitdate << "|"
            << order.createdat << "|"
            << order.totalprice << "|"
            << order.memberdiscount << "|"
            << order.holidaydiscount << "\n";
    }
    for (size_t i = 0; i < items.size(); i++) {
        const OrderItem& item = items[i];
        files[1] << item.itemid << "|"
            << item.orderid << "|"
            << tickettypecode(item.type) << "|"
            << item.quantity << "|"
            << item.unitprice << "\n";
    }
    for (size_t i = 0; i < tickets.size(); i++) {
        const AdmissionTicket& ticket = tickets[i];
        string couponid = ticket.couponid;
        if (couponid.empty()) {
            couponid = "-";
        }
        files[2] << ticket.ticketid << "|"
            << ticket.itemid << "|"
            << ticket.paidprice << "|"
            << ticketstatuscode(ticket.status) << "|"
            << couponid << "\n";
    }
    for (size_t i = 0; i < refunds.size(); i++) {
        const RefundRecord& record = refunds[i];
        files[3] << record.recordid << "|"
            << record.batchid << "|"
            << record.ticketid << "|"
            << record.operatorid << "|"
            << record.amount << "|"
            << record.refundedat << "\n";
    }
    for (size_t i = 0; i < checkins.size(); i++) {
        const CheckInRecord& record = checkins[i];
        files[4] << record.recordid << "|"
            << record.batchid << "|"
            << record.ticketid << "|"
            << record.operatorid << "|"
            << record.checkedinat << "\n";
    }
    files[5] << "default|" << defaultcapacity << "\n";
    for (size_t i = 0; i < capacities.size(); i++) {
        files[5] << capacities[i].date << "|"
            << capacities[i].capacity << "\n";
    }
    bool success = true;
    for (int i = 0; i < 6; i++) {
        files[i].close();
        if (files[i].fail()) {
            if (success) {
                lasterror = "文件写入失败：" + filenames[i];
            }
            success = false;
        }
    }
    return success;
}
///票务读取的共用解析工具
static bool readticketrows(const string& filename,
    int fieldcount,
    vector<vector<string>>& rows,
    string& error,
    int optionalfields = 0) {
    error.clear();
    ifstream file(filename);
    if (!file.is_open()) {
        error = "无法打开文件：" + filename;
        return false;
    }
    vector<vector<string>> loaded;
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
        if (fields.size() < static_cast<size_t>(fieldcount)
            || fields.size() > static_cast<size_t>(fieldcount + optionalfields)
            || line.back() == '|') {
            error = filename + "第" + to_string(linenumber) + "行字段数量错误";
            return false;
        }
        loaded.push_back(fields);
    }
    if (file.bad() || !file.eof()) {
        error = "读取文件失败：" + filename;
        return false;
    }
    rows = loaded;
    return true;
}
static bool readmoney(const string& text, Money& value) {
    if (text.empty()) {
        return false;
    }
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] < '0' || text[i] > '9') {
            return false;
        }
    }
    Money result = 0;
    stringstream number(text);
    if (!(number >> result) || !number.eof()) {
        return false;
    }
    value = result;
    return true;
}
static bool readnonnegativeint(const string& text, int& value) {
    Money result = 0;
    if (!readmoney(text, result) || result > INT_MAX) {
        return false;
    }
    value = static_cast<int>(result);
    return true;
}
static bool readtickettype(const string& text, TicketType& type) {
    if (text == "adult") {
        type = TicketType::adult;
    }
    else if (text == "student") {
        type = TicketType::student;
    }
    else if (text == "child") {
        type = TicketType::child;
    }
    else {
        return false;
    }
    return true;
}
static bool readticketstatus(const string& text, TicketStatus& status) {
    if (text == "UNUSED") {
        status = TicketStatus::unused;
    }
    else if (text == "USED") {
        status = TicketStatus::used;
    }
    else if (text == "REFUNDED") {
        status = TicketStatus::refunded;
    }
    else {
        return false;
    }
    return true;
}
///读取订单和购买明细
static bool readorders(const string& filename,
    vector<Order>& result,
    string& error) {
    vector<vector<string>> rows;
    if (!readticketrows(filename, 6, rows, error, 1)) {
        return false;
    }
    vector<Order> loaded;
    for (size_t i = 0; i < rows.size(); i++) {
        const vector<string>& fields = rows[i];
        string position = filename + "第" + to_string(i + 1) + "条记录";
        if (!FieldValidation::validid(fields[0])
            || !FieldValidation::validid(fields[1])) {
            error = position + "订单编号或游客编号格式错误";
            return false;
        }
        if (!DateUtils::validdate(fields[2])
            || !DateUtils::validdatetime(fields[3])) {
            error = position + "游玩日期或购买时间格式错误";
            return false;
        }
        Money totalprice = 0;
        if (!readmoney(fields[4], totalprice)) {
            error = position + "付款金额格式错误或超出范围";
            return false;
        }
        if (fields[5] != "0" && fields[5] != "1") {
            error = position + "会员折扣标记只能为0或1";
            return false;
        }
        Order order;
        order.orderid = fields[0];
        order.visitorid = fields[1];
        order.visitdate = fields[2];
        order.createdat = fields[3];
        order.totalprice = totalprice;
        order.memberdiscount = fields[5] == "1";
        if (fields.size() == 7) {
            if (fields[6] != "0" && fields[6] != "1") {
                error = position + "节日折扣标记只能为0或1";
                return false;
            }
            order.holidaydiscount = fields[6] == "1";
        }
        loaded.push_back(order);
    }
    result = loaded;
    return true;
}
static bool readitems(const string& filename,
    vector<OrderItem>& result,
    string& error) {
    vector<vector<string>> rows;
    if (!readticketrows(filename, 5, rows, error)) {
        return false;
    }
    vector<OrderItem> loaded;
    for (size_t i = 0; i < rows.size(); i++) {
        const vector<string>& fields = rows[i];
        string position = filename + "第" + to_string(i + 1) + "条记录";
        if (!FieldValidation::validid(fields[0])
            || !FieldValidation::validid(fields[1])) {
            error = position + "明细编号或订单编号格式错误";
            return false;
        }
        TicketType type = TicketType::adult;
        if (!readtickettype(fields[2], type)) {
            error = position + "票种错误";
            return false;
        }
        int quantity = 0;
        if (!readnonnegativeint(fields[3], quantity)
            || quantity < 1 || quantity > 10) {
            error = position + "购买数量必须为1至10张";
            return false;
        }
        Money unitprice = 0;
        if (!readmoney(fields[4], unitprice) || unitprice <= 0) {
            error = position + "基础单价必须为大于零的整数分";
            return false;
        }
        OrderItem item;
        item.itemid = fields[0];
        item.orderid = fields[1];
        item.type = type;
        item.quantity = quantity;
        item.unitprice = unitprice;
        loaded.push_back(item);
    }
    result = loaded;
    return true;
}
///读取电子票、退款和检票记录
static bool readtickets(const string& filename,
    vector<AdmissionTicket>& result,
    string& error) {
    vector<vector<string>> rows;
    if (!readticketrows(filename, 5, rows, error)) {
        return false;
    }
    vector<AdmissionTicket> loaded;
    for (size_t i = 0; i < rows.size(); i++) {
        const vector<string>& fields = rows[i];
        string position = filename + "第" + to_string(i + 1) + "条记录";
        if (!FieldValidation::validid(fields[0])
            || !FieldValidation::validid(fields[1])) {
            error = position + "电子票编号或明细编号格式错误";
            return false;
        }
        Money paidprice = 0;
        if (!readmoney(fields[2], paidprice)) {
            error = position + "实付金额格式错误或超出范围";
            return false;
        }
        TicketStatus status = TicketStatus::unused;
        if (!readticketstatus(fields[3], status)) {
            error = position + "电子票状态错误";
            return false;
        }
        if (fields[4] != "-" && !FieldValidation::validid(fields[4])) {
            error = position + "优惠券编号格式错误";
            return false;
        }
        AdmissionTicket ticket;
        ticket.ticketid = fields[0];
        ticket.itemid = fields[1];
        ticket.paidprice = paidprice;
        ticket.status = status;
        if (fields[4] != "-") {
            ticket.couponid = fields[4];
        }
        loaded.push_back(ticket);
    }
    result = loaded;
    return true;
}
static bool readrefunds(const string& filename,
    vector<RefundRecord>& result,
    string& error) {
    vector<vector<string>> rows;
    if (!readticketrows(filename, 6, rows, error)) {
        return false;
    }
    vector<RefundRecord> loaded;
    for (size_t i = 0; i < rows.size(); i++) {
        const vector<string>& fields = rows[i];
        string position = filename + "第" + to_string(i + 1) + "条记录";
        for (int j = 0; j < 4; j++) {
            if (!FieldValidation::validid(fields[j])) {
                error = position + "退款记录中的编号格式错误";
                return false;
            }
        }
        Money amount = 0;
        if (!readmoney(fields[4], amount)) {
            error = position + "退款金额格式错误或超出范围";
            return false;
        }
        if (!DateUtils::validdatetime(fields[5])) {
            error = position + "退款时间格式错误";
            return false;
        }
        RefundRecord record;
        record.recordid = fields[0];
        record.batchid = fields[1];
        record.ticketid = fields[2];
        record.operatorid = fields[3];
        record.amount = amount;
        record.refundedat = fields[5];
        loaded.push_back(record);
    }
    result = loaded;
    return true;
}
static bool readcheckins(const string& filename,
    vector<CheckInRecord>& result,
    string& error) {
    vector<vector<string>> rows;
    if (!readticketrows(filename, 5, rows, error)) {
        return false;
    }
    vector<CheckInRecord> loaded;
    for (size_t i = 0; i < rows.size(); i++) {
        const vector<string>& fields = rows[i];
        string position = filename + "第" + to_string(i + 1) + "条记录";
        for (int j = 0; j < 4; j++) {
            if (!FieldValidation::validid(fields[j])) {
                error = position + "检票记录中的编号格式错误";
                return false;
            }
        }
        if (!DateUtils::validdatetime(fields[4])) {
            error = position + "检票时间格式错误";
            return false;
        }
        CheckInRecord record;
        record.recordid = fields[0];
        record.batchid = fields[1];
        record.ticketid = fields[2];
        record.operatorid = fields[3];
        record.checkedinat = fields[4];
        loaded.push_back(record);
    }
    result = loaded;
    return true;
}
///读取容量并建立统一加载入口
static bool readcapacities(const string& filename,
    int& defaultcapacity,
    vector<DailyCapacity>& result,
    string& error) {
    vector<vector<string>> rows;
    if (!readticketrows(filename, 2, rows, error)) {
        return false;
    }
    int loadeddefault = 0;
    bool founddefault = false;
    vector<DailyCapacity> loaded;
    for (size_t i = 0; i < rows.size(); i++) {
        const vector<string>& fields = rows[i];
        string position = filename + "第" + to_string(i + 1) + "条记录";
        int capacity = 0;
        if (!readnonnegativeint(fields[1], capacity)) {
            error = position + "容量必须为范围内的非负整数";
            return false;
        }
        if (fields[0] == "default") {
            if (founddefault) {
                error = position + "默认容量重复";
                return false;
            }
            loadeddefault = capacity;
            founddefault = true;
        }
        else {
            if (!DateUtils::validdate(fields[0])) {
                error = position + "容量设置日期格式错误";
                return false;
            }
            for (size_t j = 0; j < loaded.size(); j++) {
                if (loaded[j].date == fields[0]) {
                    error = position + "容量设置日期重复";
                    return false;
                }
            }
            DailyCapacity setting;
            setting.date = fields[0];
            setting.capacity = capacity;
            loaded.push_back(setting);
        }
    }
    if (!founddefault) {
        error = "容量文件缺少默认容量";
        return false;
    }
    defaultcapacity = loadeddefault;
    result = loaded;
    return true;
}
bool TicketManager::loadfromfiles(const string& directory) {
    lasterror.clear();
    if (directory.empty()) {
        lasterror = "加载目录不能为空";
        return false;
    }
    string prefix = directory;
    if (prefix.back() != '/' && prefix.back() != '\\') {
        prefix += "/";
    }
    TicketManager loaded(visitors, catalog, coupons);
    if (!readorders(prefix + "orders.txt", loaded.orders, lasterror)
        || !readitems(prefix + "order_items.txt", loaded.items, lasterror)
        || !readtickets(prefix + "tickets.txt", loaded.tickets, lasterror)
        || !readrefunds(prefix + "refund_records.txt", loaded.refunds, lasterror)
        || !readcheckins(prefix + "checkin_records.txt", loaded.checkins, lasterror)
        || !readcapacities(prefix + "capacity_settings.txt",
            loaded.defaultcapacity, loaded.capacities, lasterror)) {
        return false;
    }
    if (!loaded.validateorders()
        || !loaded.validateoperations()
        || !loaded.validatecapacity()) {
        lasterror = loaded.getlasterror();
        return false;
    }
    orders.swap(loaded.orders);
    items.swap(loaded.items);
    tickets.swap(loaded.tickets);
    refunds.swap(loaded.refunds);
    checkins.swap(loaded.checkins);
    capacities.swap(loaded.capacities);
    defaultcapacity = loaded.defaultcapacity;
    return true;
}
///编号检查
static bool addrecordid(vector<string>& ids, const string& id) {
    for (size_t i = 0; i < ids.size(); i++) {
        if (ids[i] == id) {
            return false;
        }
    }
    ids.push_back(id);
    return true;
}
///关联、数量、金额检查
bool TicketManager::validateorders() {
    lasterror.clear();
    vector<string> ids;
    for (size_t i = 0; i < orders.size(); i++) {
        if (!addrecordid(ids, orders[i].orderid)) {
            lasterror = "订单编号重复";
            return false;
        }
    }
    for (size_t i = 0; i < items.size(); i++) {
        if (!addrecordid(ids, items[i].itemid)) {
            lasterror = "购买明细编号与已有记录重复";
            return false;
        }
        if (findorder(items[i].orderid) == -1) {
            lasterror = "购买明细对应的订单不存在";
            return false;
        }
    }
    for (size_t i = 0; i < tickets.size(); i++) {
        if (!addrecordid(ids, tickets[i].ticketid)) {
            lasterror = "电子票编号与已有记录重复";
            return false;
        }
        if (finditem(tickets[i].itemid) == -1) {
            lasterror = "电子票对应的购买明细不存在";
            return false;
        }
    }
    for (size_t i = 0; i < refunds.size(); i++) {
        if (!addrecordid(ids, refunds[i].recordid)) {
            lasterror = "退款记录编号与已有记录重复";
            return false;
        }
    }
    for (size_t i = 0; i < checkins.size(); i++) {
        if (!addrecordid(ids, checkins[i].recordid)) {
            lasterror = "检票记录编号与已有记录重复";
            return false;
        }
    }
    Session system(Role::admin, "ADMIN", "系统加载");
    for (size_t i = 0; i < orders.size(); i++) {
        const Order& order = orders[i];
        if (order.memberdiscount && order.holidaydiscount) {
            lasterror = "节日折扣不能与会员折扣同时使用";
            return false;
        }
        Visitor visitor;
        if (!visitors.getvisitorinfo(system, order.visitorid, visitor)) {
            lasterror = "订单对应的游客不存在";
            return false;
        }
        string purchasedate = order.createdat.substr(0, 10);
        string lastdate = DateUtils::adddays(purchasedate, 30);
        if (lastdate.empty()
            || order.visitdate < purchasedate
            || order.visitdate > lastdate) {
            lasterror = "订单游玩日期不在购买时的可售范围内";
            return false;
        }
        int quantity = 0;
        int itemcount = 0;
        int couponcount = 0;
        Money originaltotal = 0;
        Money totalprice = 0;
        vector<TicketType> types;
        // 检查订单内各条明细及实际出票
        for (size_t j = 0; j < items.size(); j++) {
            const OrderItem& item = items[j];
            if (item.orderid != order.orderid) {
                continue;
            }
            itemcount++;
            for (size_t k = 0; k < types.size(); k++) {
                if (types[k] == item.type) {
                    lasterror = "同一订单出现重复票种明细";
                    return false;
                }
            }
            types.push_back(item.type);
            quantity += item.quantity;
            if (quantity > 10) {
                lasterror = "订单购买数量超过10张";
                return false;
            }
            if (item.unitprice > (LLONG_MAX - originaltotal) / item.quantity) {
                lasterror = "订单基础金额超出范围";
                return false;
            }
            originaltotal += item.unitprice * item.quantity;
            Money unitprice = item.unitprice;
            if (order.holidaydiscount) {
                unitprice = item.unitprice / 10 * 8
                    + item.unitprice % 10 * 8 / 10;
            }
            else if (order.memberdiscount) {
                unitprice = item.unitprice / 10 * 9
                    + item.unitprice % 10 * 9 / 10;
            }
            int ticketcount = 0;
            for (size_t k = 0; k < tickets.size(); k++) {
                const AdmissionTicket& ticket = tickets[k];
                if (ticket.itemid != item.itemid) {
                    continue;
                }
                ticketcount++;
                Money expectedprice = unitprice;
                if (!ticket.couponid.empty()) {
                    couponcount++;
                    if (order.memberdiscount || order.holidaydiscount || couponcount > 1) {
                        lasterror = "订单优惠券数量或优惠叠加规则错误";
                        return false;
                    }
                    Coupon coupon;
                    if (!coupons.getcouponinfo(system, ticket.couponid, coupon)) {
                        lasterror = "电子票对应的优惠券不存在";
                        return false;
                    }
                    if (coupon.visitorid != order.visitorid
                        || coupon.status != CouponStatus::used
                        || coupon.ticketid != ticket.ticketid) {
                        lasterror = "优惠券归属或使用记录与电子票不一致";
                        return false;
                    }
                    if (coupon.issuedat > order.createdat
                        || coupon.usedat < order.createdat) {
                        lasterror = "优惠券发放或使用时间与订单不一致";
                        return false;
                    }
                    if (expectedprice < coupon.amount) {
                        lasterror = "优惠券金额超过电子票基础价格";
                        return false;
                    }
                    expectedprice -= coupon.amount;
                }
                if (ticket.paidprice != expectedprice) {
                    lasterror = "电子票实付金额与优惠规则不一致";
                    return false;
                }
                if (ticket.paidprice > LLONG_MAX - totalprice) {
                    lasterror = "订单实付金额合计超出范围";
                    return false;
                }
                totalprice += ticket.paidprice;
            }
            if (ticketcount != item.quantity) {
                lasterror = "明细数量与实际电子票数量不一致";
                return false;
            }
        }
        if (itemcount < 1 || itemcount > 3 || quantity < 1) {
            lasterror = "订单缺少购买明细或明细数量错误";
            return false;
        }
        if (totalprice != order.totalprice) {
            lasterror = "订单总额与电子票实付金额合计不一致";
            return false;
        }
    }
    return true;
}
///票务数据的保存与加载
bool TicketManager::validateoperations() {
    // 检查退款记录
    for (size_t i = 0; i < refunds.size(); i++) {
        const RefundRecord& record = refunds[i];
        int ticketindex = findticket(record.ticketid);
        if (ticketindex == -1) {
            lasterror = "退款记录对应的门票不存在";
            return false;
        }
        const AdmissionTicket& ticket = tickets[ticketindex];
        int itemindex = finditem(ticket.itemid);
        int orderindex = findorder(items[itemindex].orderid);
        const Order& order = orders[orderindex];
        if (record.operatorid != order.visitorid) {
            lasterror = "退款操作人和订单所属游客不一致";
            return false;
        }
        if (record.amount != ticket.paidprice) {
            lasterror = "退款金额和门票实付金额不一致";
            return false;
        }
        if (record.refundedat < order.createdat ||
            record.refundedat.substr(0, 10) >= order.visitdate) {
            lasterror = "退款时间不符合要求";
            return false;
        }
        // 同一退款批次应属于同一订单，并使用相同的操作人和时间
        for (size_t j = 0; j < i; j++) {
            if (refunds[j].batchid == record.batchid) {
                int previousindex = findticket(refunds[j].ticketid);
                int previousitem = finditem(tickets[previousindex].itemid);
                if (items[previousitem].orderid != order.orderid ||
                    refunds[j].operatorid != record.operatorid ||
                    refunds[j].refundedat != record.refundedat) {
                    lasterror = "同一退款批次的记录不一致";
                    return false;
                }
            }
        }
    }
    // 检查入园记录
    for (size_t i = 0; i < checkins.size(); i++) {
        const CheckInRecord& record = checkins[i];
        int ticketindex = findticket(record.ticketid);
        if (ticketindex == -1) {
            lasterror = "入园记录对应的门票不存在";
            return false;
        }
        int itemindex = finditem(tickets[ticketindex].itemid);
        int orderindex = findorder(items[itemindex].orderid);
        const Order& order = orders[orderindex];
        if (record.operatorid != "ADMIN") {
            lasterror = "入园记录的操作人不是系统管理员";
            return false;
        }
        if (record.checkedinat < order.createdat ||
            record.checkedinat.substr(0, 10) != order.visitdate) {
            lasterror = "入园时间和门票游览日期不一致";
            return false;
        }
        // 同一入园批次应属于同一订单，并使用相同的操作人和时间
        for (size_t j = 0; j < i; j++) {
            if (checkins[j].batchid == record.batchid) {
                int previousindex = findticket(checkins[j].ticketid);
                int previousitem = finditem(tickets[previousindex].itemid);
                if (items[previousitem].orderid != order.orderid ||
                    checkins[j].operatorid != record.operatorid ||
                    checkins[j].checkedinat != record.checkedinat) {
                    lasterror = "同一入园批次的记录不一致";
                    return false;
                }
            }
        }
    }
    // 检查每张门票的状态与操作记录
    for (size_t i = 0; i < tickets.size(); i++) {
        int refundcount = 0;
        int checkincount = 0;
        for (size_t j = 0; j < refunds.size(); j++) {
            if (refunds[j].ticketid == tickets[i].ticketid) {
                refundcount++;
            }
        }
        for (size_t j = 0; j < checkins.size(); j++) {
            if (checkins[j].ticketid == tickets[i].ticketid) {
                checkincount++;
            }
        }
        bool correct = false;
        if (tickets[i].status == TicketStatus::unused) {
            correct = refundcount == 0 && checkincount == 0;
        }
        else if (tickets[i].status == TicketStatus::used) {
            correct = refundcount == 0 && checkincount == 1;
        }
        else if (tickets[i].status == TicketStatus::refunded) {
            correct = refundcount == 1 && checkincount == 0;
        }
        if (!correct) {
            lasterror = "门票状态和退款、入园记录不一致";
            return false;
        }
    }
    // 收集已占用的编号
    vector<string> recordids;
    for (size_t i = 0; i < orders.size(); i++) {
        recordids.push_back(orders[i].orderid);
    }
    for (size_t i = 0; i < items.size(); i++) {
        recordids.push_back(items[i].itemid);
    }
    for (size_t i = 0; i < tickets.size(); i++) {
        recordids.push_back(tickets[i].ticketid);
    }
    for (size_t i = 0; i < refunds.size(); i++) {
        recordids.push_back(refunds[i].recordid);
    }
    for (size_t i = 0; i < checkins.size(); i++) {
        recordids.push_back(checkins[i].recordid);
    }
    // 批次编号不能与其他记录编号重复
    for (size_t i = 0; i < recordids.size(); i++) {
        for (size_t j = 0; j < refunds.size(); j++) {
            if (recordids[i] == refunds[j].batchid) {
                lasterror = "退款批次编号和其他记录编号重复";
                return false;
            }
        }
        for (size_t j = 0; j < checkins.size(); j++) {
            if (recordids[i] == checkins[j].batchid) {
                lasterror = "入园批次编号和其他记录编号重复";
                return false;
            }
        }
    }
    // 退款和入园不能共用批次编号
    for (size_t i = 0; i < refunds.size(); i++) {
        for (size_t j = 0; j < checkins.size(); j++) {
            if (refunds[i].batchid == checkins[j].batchid) {
                lasterror = "退款和入园的批次编号重复";
                return false;
            }
        }
    }
    return true;
}
///容量检查函数
bool TicketManager::validatecapacity() {
    if (defaultcapacity < 0) {
        lasterror = "默认接待容量不能为负数";
        return false;
    }
    for (size_t i = 0; i < capacities.size(); i++) {
        if (capacities[i].capacity < 0) {
            lasterror = "指定日期的接待容量不能为负数";
            return false;
        }
    }
    // 检查各游览日期的有效门票数量
    for (size_t i = 0; i < orders.size(); i++) {
        const string& date = orders[i].visitdate;
        int reserved = countreserved(date);
        int capacity = getcapacity(date);
        if (reserved > capacity) {
            lasterror = date + " 的有效门票数量超过接待容量";
            return false;
        }
    }
    return true;
}