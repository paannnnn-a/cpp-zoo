#pragma once
#include <string>
#include "session.h"
using namespace std;
/// 将操作记录追加到日志文件
class OperationLogger {
private:
    string filename;   ///< 日志文件路径
    string lasterror;  ///< 最近一次写入失败的说明
public:
    /// 指定日志文件路径
    OperationLogger(const string& logfile);
    /// 记录操作人、操作名称、相关编号及结果
    bool writelog(const Session& session,
        const string& action,
        const string& targetid,
        bool success,
        const string& detail = "");
    /// 获取最近一次写入失败的说明
    string getlasterror() const;
};