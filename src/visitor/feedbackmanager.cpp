#include "visitor/feedbackmanager.h"
#include "visitor/visitormanager.h"
#include "visitor/ticketmanager.h"
#include "visitor/visitorutils.h"
#include <sstream>
#include <iomanip>
#include <fstream>
using namespace std;

///反馈管理
FeedbackManager::FeedbackManager(VisitorManager& visitormanager,
    const TicketManager& ticketmanager)
    : visitors(visitormanager), tickets(ticketmanager) {
}
int FeedbackManager::findfeedback(const string& feedbackid) const {
    for (size_t i = 0; i < feedbacks.size(); i++) {
        if (feedbacks[i].feedbackid == feedbackid) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
string FeedbackManager::generateid() const {
    for (int number = 1; number <= 99999999; number++) {
        ostringstream result;
        result << "F" << setfill('0') << setw(4) << number;
        string id = result.str();
        if (findfeedback(id) == -1) {
            return id;
        }
    }
    return "";
}
bool FeedbackManager::checkvisitor(const Session& session) {
    if (session.getrole() != Role::visitor) {
        lasterror = "请使用游客账号提交或查询个人反馈";
        return false;
    }
    Visitor visitor;
    if (!visitors.getvisitorinfo(session, session.getuserid(), visitor)) {
        lasterror = visitors.getlasterror();
        return false;
    }
    if (!visitor.getactive()) {
        lasterror = "游客账号已停用";
        return false;
    }
    return true;
}
bool FeedbackManager::submitsuggestion(const Session& session,
    const string& content,
    string& feedbackid) {
    lasterror.clear();
    if (!checkvisitor(session)) {
        return false;
    }
    if (!FieldValidation::validtext(content)) {
        lasterror = "建议内容不能为空或全为空白，也不能包含竖线、换行或空字符";
        return false;
    }
    string current = DateUtils::now();
    if (current.empty()) {
        lasterror = "无法获取当前时间";
        return false;
    }
    string id = generateid();
    if (id.empty()) {
        lasterror = "反馈编号已用完";
        return false;
    }
    Feedback feedback;
    feedback.feedbackid = id;
    feedback.visitorid = session.getuserid();
    feedback.content = content;
    feedback.createdat = current;
    feedbacks.push_back(feedback);
    feedbackid = id;
    return true;
}



bool FeedbackManager::submitreview(const Session& session,
    const string& visitdate,
    int rating,
    const string& content,
    string& feedbackid) {
    lasterror.clear();
    if (!checkvisitor(session)) {
        return false;
    }
    if (!DateUtils::validdate(visitdate)) {
        lasterror = "游览日期必须是合法的YYYY-MM-DD日期";
        return false;
    }
    if (rating < 1 || rating > 5) {
        lasterror = "评价评分必须为1至5";
        return false;
    }
    if (!FieldValidation::validtext(content)) {
        lasterror = "评价内容不能为空或全为空白，也不能包含竖线、换行或空字符";
        return false;
    }
    string current = DateUtils::now();
    if (current.empty()) {
        lasterror = "无法获取当前时间";
        return false;
    }
    if (visitdate > current.substr(0, 10)) {
        lasterror = "不能评价未来的游览日期";
        return false;
    }
    if (!tickets.hasvisited(session.getuserid(), visitdate)) {
        lasterror = "该日期没有实际入园记录，不能提交游后评价";
        return false;
    }
    // 检查同一游客是否已经评价过该游览日期
    for (size_t i = 0; i < feedbacks.size(); i++) {
        if (feedbacks[i].visitorid == session.getuserid()
            && feedbacks[i].type == FeedbackType::review
            && feedbacks[i].visitdate == visitdate) {
            lasterror = "你已经评价过该游览日期";
            return false;
        }
    }
    string id = generateid();
    if (id.empty()) {
        lasterror = "反馈编号已用完";
        return false;
    }
    Feedback feedback;
    feedback.feedbackid = id;
    feedback.visitorid = session.getuserid();
    feedback.type = FeedbackType::review;
    feedback.visitdate = visitdate;
    feedback.rating = rating;
    feedback.content = content;
    feedback.createdat = current;
    feedbacks.push_back(feedback);
    feedbackid = id;
    return true;
}
///反馈列表和单挑反馈查询

bool FeedbackManager::getfeedbacks(const Session& session,
    const FeedbackQuery& query,
    vector<Feedback>& result) {
    lasterror.clear();
    if (session.getrole() == Role::visitor) {
        if (!checkvisitor(session)) {
            return false;
        }
        if (!query.visitorid.empty()
            && query.visitorid != session.getuserid()) {
            lasterror = "只能查询本人的反馈";
            return false;
        }
    }
    else if (session.getrole() != Role::admin) {
        lasterror = "请先登录";
        return false;
    }
    if (!query.visitorid.empty()
        && !FieldValidation::validid(query.visitorid)) {
        lasterror = "游客编号格式错误";
        return false;
    }
    if (!query.visitdate.empty()
        && !DateUtils::validdate(query.visitdate)) {
        lasterror = "游览日期必须是合法的YYYY-MM-DD日期";
        return false;
    }
    if (query.filtertype
        && query.type != FeedbackType::suggestion
        && query.type != FeedbackType::review) {
        lasterror = "反馈种类错误";
        return false;
    }
    if (query.filterstatus
        && query.status != FeedbackStatus::pending
        && query.status != FeedbackStatus::processing
        && query.status != FeedbackStatus::resolved) {
        lasterror = "反馈处理状态错误";
        return false;
    }
    vector<Feedback> matched;
    for (size_t i = 0; i < feedbacks.size(); i++) {
        const Feedback& feedback = feedbacks[i];
        if (session.getrole() == Role::visitor
            && feedback.visitorid != session.getuserid()) {
            continue;
        }
        if (!query.visitorid.empty()
            && feedback.visitorid != query.visitorid) {
            continue;
        }
        if (!query.visitdate.empty()
            && feedback.visitdate != query.visitdate) {
            continue;
        }
        if (query.filtertype && feedback.type != query.type) {
            continue;
        }
        if (query.filterstatus && feedback.status != query.status) {
            continue;
        }
        matched.push_back(feedback);
    }
    result = matched;
    return true;
}
bool FeedbackManager::getfeedbackdetail(const Session& session,
    const string& feedbackid,
    Feedback& result) {
    lasterror.clear();
    if (session.getrole() == Role::visitor) {
        if (!checkvisitor(session)) {
            return false;
        }
    }
    else if (session.getrole() != Role::admin) {
        lasterror = "请先登录";
        return false;
    }
    int index = findfeedback(feedbackid);
    if (index == -1) {
        lasterror = "反馈不存在";
        return false;
    }
    if (session.getrole() == Role::visitor
        && feedbacks[index].visitorid != session.getuserid()) {
        lasterror = "只能查询本人的反馈";
        return false;
    }
    result = feedbacks[index];
    return true;
}
///管理员处理反馈
bool FeedbackManager::handlefeedback(const Session& session,
    const string& feedbackid,
    FeedbackStatus status,
    const string& reply) {
    lasterror.clear();
    if (session.getrole() != Role::admin) {
        lasterror = "只有管理员可以处理反馈";
        return false;
    }
    int index = findfeedback(feedbackid);
    if (index == -1) {
        lasterror = "反馈不存在";
        return false;
    }
    if (status != FeedbackStatus::processing
        && status != FeedbackStatus::resolved) {
        lasterror = "只能设置为处理中或已处理";
        return false;
    }
    if (feedbacks[index].status == FeedbackStatus::resolved
        && status == FeedbackStatus::processing) {
        lasterror = "已处理的反馈不能退回处理中";
        return false;
    }
    if (!reply.empty() && !FieldValidation::validtext(reply)) {
        lasterror = "回复不能全为空白，也不能包含竖线、换行或空字符";
        return false;
    }
    if (status == FeedbackStatus::resolved && reply.empty()) {
        lasterror = "标记为已处理时必须填写回复";
        return false;
    }
    string current = DateUtils::now();
    if (current.empty()) {
        lasterror = "无法获取当前时间";
        return false;
    }
    if (current < feedbacks[index].createdat
        || (!feedbacks[index].handledat.empty()
            && current < feedbacks[index].handledat)) {
        lasterror = "当前时间早于反馈提交或上次处理时间";
        return false;
    }
    feedbacks[index].status = status;
    feedbacks[index].reply = reply;
    feedbacks[index].operatorid = session.getuserid();
    feedbacks[index].handledat = current;
    return true;
}
///保存反馈至文件
static string feedbacktypecode(FeedbackType type) {
    if (type == FeedbackType::suggestion) {
        return "SUGGESTION";
    }
    if (type == FeedbackType::review) {
        return "REVIEW";
    }
    return "";
}
static string feedbackstatuscode(FeedbackStatus status) {
    if (status == FeedbackStatus::pending) {
        return "PENDING";
    }
    if (status == FeedbackStatus::processing) {
        return "PROCESSING";
    }
    if (status == FeedbackStatus::resolved) {
        return "RESOLVED";
    }
    return "";
}
bool FeedbackManager::savetofile(const string& filename) {
    lasterror.clear();
    for (size_t i = 0; i < feedbacks.size(); i++) {
        if (feedbacktypecode(feedbacks[i].type).empty()
            || feedbackstatuscode(feedbacks[i].status).empty()) {
            lasterror = "反馈种类或处理状态错误";
            return false;
        }
    }
    ofstream file(filename);
    if (!file.is_open()) {
        lasterror = "无法打开反馈文件：" + filename;
        return false;
    }
    for (size_t i = 0; i < feedbacks.size(); i++) {
        const Feedback& feedback = feedbacks[i];
        string visitdate = feedback.visitdate;
        string operatorid = feedback.operatorid;
        string handledat = feedback.handledat;
        if (visitdate.empty()) {
            visitdate = "-";
        }
        if (operatorid.empty()) {
            operatorid = "-";
        }
        if (handledat.empty()) {
            handledat = "-";
        }
        file << feedback.feedbackid << "|"
            << feedback.visitorid << "|"
            << feedbacktypecode(feedback.type) << "|"
            << visitdate << "|"
            << feedback.rating << "|"
            << feedback.content << "|"
            << feedback.createdat << "|"
            << feedbackstatuscode(feedback.status) << "|"
            << feedback.reply << "|"
            << operatorid << "|"
            << handledat << "\n";
    }
    file.close();
    if (file.fail()) {
        lasterror = "反馈文件写入失败：" + filename;
        return false;
    }
    return true;
}
///读取文件
static bool readfeedbacktype(const string& text, FeedbackType& type) {
    if (text == "SUGGESTION") {
        type = FeedbackType::suggestion;
    }
    else if (text == "REVIEW") {
        type = FeedbackType::review;
    }
    else {
        return false;
    }
    return true;
}
static bool readfeedbackstatus(const string& text,
    FeedbackStatus& status) {
    if (text == "PENDING") {
        status = FeedbackStatus::pending;
    }
    else if (text == "PROCESSING") {
        status = FeedbackStatus::processing;
    }
    else if (text == "RESOLVED") {
        status = FeedbackStatus::resolved;
    }
    else {
        return false;
    }
    return true;
}
static bool readfeedbackrating(const string& text, int& rating) {
    if (text.size() != 1 || text[0] < '0' || text[0] > '5') {
        return false;
    }
    rating = text[0] - '0';
    return true;
}
static bool readfeedbackfields(const string& line,
    vector<string>& fields) {
    if (line.empty() || line.back() == '|') {
        return false;
    }
    vector<string> loaded;
    string field;
    stringstream row(line);
    while (getline(row, field, '|')) {
        loaded.push_back(field);
    }
    if (loaded.size() != 11) {
        return false;
    }
    fields = loaded;
    return true;
}
bool FeedbackManager::loadfromfile(const string& filename) {
    lasterror.clear();
    ifstream file(filename);
    if (!file.is_open()) {
        lasterror = "无法打开反馈文件：" + filename;
        return false;
    }
    vector<Feedback> loaded;
    Session system(Role::admin, "ADMIN", "系统加载");
    string line;
    int linenumber = 0;
    while (getline(file, line)) {
        linenumber++;
        if (linenumber == 1
            && line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }
        string position = "反馈文件第" + to_string(linenumber) + "行：";
        vector<string> fields;
        if (!readfeedbackfields(line, fields)) {
            lasterror = position + "必须包含11个字段";
            return false;
        }
        Feedback feedback;
        if (!FieldValidation::validid(fields[0])
            || !FieldValidation::validid(fields[1])) {
            lasterror = position + "反馈编号或游客编号格式错误";
            return false;
        }
        if (!readfeedbacktype(fields[2], feedback.type)
            || !readfeedbackrating(fields[4], feedback.rating)
            || !readfeedbackstatus(fields[7], feedback.status)) {
            lasterror = position + "反馈种类、评分或处理状态错误";
            return false;
        }
        if (!FieldValidation::validtext(fields[5])
            || !DateUtils::validdatetime(fields[6])) {
            lasterror = position + "反馈内容或提交时间错误";
            return false;
        }
        if (!fields[8].empty()
            && !FieldValidation::validtext(fields[8])) {
            lasterror = position + "回复内容格式错误";
            return false;
        }
        feedback.feedbackid = fields[0];
        feedback.visitorid = fields[1];
        feedback.content = fields[5];
        feedback.createdat = fields[6];
        feedback.reply = fields[8];
        // 检查反馈对应的游客
        Visitor visitor;
        if (!visitors.getvisitorinfo(system, feedback.visitorid, visitor)) {
            lasterror = position + "对应的游客不存在";
            return false;
        }
        // 检查建议与评价各自的字段
        if (feedback.type == FeedbackType::suggestion) {
            if (fields[3] != "-" || feedback.rating != 0) {
                lasterror = position + "建议不能填写游览日期或评分";
                return false;
            }
        }
        else {
            if (!DateUtils::validdate(fields[3])
                || feedback.rating < 1 || feedback.rating > 5) {
                lasterror = position + "评价日期或评分错误";
                return false;
            }
            feedback.visitdate = fields[3];
            if (feedback.visitdate > feedback.createdat.substr(0, 10)) {
                lasterror = position + "评价提交时间早于游览日期";
                return false;
            }
            if (!tickets.hasvisited(feedback.visitorid, feedback.visitdate)) {
                lasterror = position + "评价对应的实际入园记录不存在";
                return false;
            }
        }
        // 检查处理状态与回复、处理人、处理时间
        if (feedback.status == FeedbackStatus::pending) {
            if (!feedback.reply.empty()
                || fields[9] != "-" || fields[10] != "-") {
                lasterror = position + "未处理的反馈不能包含处理结果";
                return false;
            }
        }
        else {
            if (fields[9] != "ADMIN"
                || !DateUtils::validdatetime(fields[10])
                || fields[10] < feedback.createdat) {
                lasterror = position + "处理人或处理时间错误";
                return false;
            }
            if (feedback.status == FeedbackStatus::resolved
                && feedback.reply.empty()) {
                lasterror = position + "已处理的反馈必须包含回复";
                return false;
            }
            feedback.operatorid = fields[9];
            feedback.handledat = fields[10];
        }
        // 检查重复编号与重复评价
        for (size_t i = 0; i < loaded.size(); i++) {
            if (loaded[i].feedbackid == feedback.feedbackid) {
                lasterror = position + "反馈编号重复";
                return false;
            }
            if (feedback.type == FeedbackType::review
                && loaded[i].type == FeedbackType::review
                && loaded[i].visitorid == feedback.visitorid
                && loaded[i].visitdate == feedback.visitdate) {
                lasterror = position + "同一游客对同一天重复评价";
                return false;
            }
        }
        loaded.push_back(feedback);
    }
    if (file.bad() || !file.eof()) {
        lasterror = "反馈文件读取失败：" + filename;
        return false;
    }
    feedbacks.swap(loaded);
    return true;
}
string FeedbackManager::getlasterror() const {
    return lasterror;
}