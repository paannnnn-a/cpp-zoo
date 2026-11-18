#include "visitor/operationlogger.h"
#include "visitor/visitorutils.h"
#include <fstream>
using namespace std;

OperationLogger::OperationLogger(const string& logfile)
    : filename(logfile) {
}
string OperationLogger::getlasterror() const {
    return lasterror;
}
bool OperationLogger::writelog(const Session& session,
    const string& action,
    const string& targetid,
    bool success,
    const string& detail) {
    lasterror.clear();
    string role;
    string operatorid;
    if (session.getrole() == Role::guest) {
        role = "GUEST";
        operatorid = "GUEST";
    }
    else if (session.getrole() == Role::visitor) {
        role = "VISITOR";
        operatorid = session.getuserid();
    }
    else if (session.getrole() == Role::admin) {
        role = "ADMIN";
        operatorid = session.getuserid();
    }
    else {
        lasterror = "操作人角色错误";
        return false;
    }
    if (!FieldValidation::validid(operatorid)
        || !FieldValidation::validid(action)) {
        lasterror = "操作人编号或操作名称格式错误";
        return false;
    }
    if (!targetid.empty() && !FieldValidation::validid(targetid)) {
        lasterror = "相关编号格式错误";
        return false;
    }
    if (!detail.empty() && !FieldValidation::validtext(detail)) {
        lasterror = "日志说明格式错误";
        return false;
    }
    string current = DateUtils::now();
    if (current.empty()) {
        lasterror = "无法获取当前时间";
        return false;
    }
    string target = targetid;
    string description = detail;
    if (target.empty()) {
        target = "-";
    }
    if (description.empty()) {
        description = "-";
    }
    string result = success ? "SUCCESS" : "FAILURE";
    ofstream file(filename, ios::app);
    if (!file.is_open()) {
        lasterror = "无法打开日志文件：" + filename;
        return false;
    }
    file << current << "|"
        << operatorid << "|"
        << role << "|"
        << action << "|"
        << target << "|"
        << result << "|"
        << description << "\n";
    file.close();
    if (file.fail()) {
        lasterror = "日志文件写入失败：" + filename;
        return false;
    }
    return true;
}