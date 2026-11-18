#pragma once
#include <string>
#include <vector>
#include "visitorcommon.h"
#include "session.h"
using namespace std;
class VisitorManager;
class TicketManager;
/// 游客建议或游后评价
struct Feedback {
    string feedbackid;                 ///< 反馈编号
    string visitorid;                  ///< 游客编号
    FeedbackType type = FeedbackType::suggestion;
    string visitdate;                  ///< 评价对应的游览日期
    int rating = 0;                     ///< 评分，建议使用0
    string content;                    ///< 反馈内容
    string createdat;                  ///< 提交时间
    FeedbackStatus status = FeedbackStatus::pending;
    string reply;                      ///< 管理员回复
    string operatorid;                 ///< 处理人编号
    string handledat;                  ///< 最近处理时间
};
/// 反馈查询条件
struct FeedbackQuery {
    string visitorid;                  ///< 按游客编号筛选
    string visitdate;                  ///< 按游览日期筛选
    bool filtertype = false;           ///< 是否筛选反馈种类
    FeedbackType type = FeedbackType::suggestion;
    bool filterstatus = false;         ///< 是否筛选处理状态
    FeedbackStatus status = FeedbackStatus::pending;
};
/// 管理建议、评价及管理员处理
class FeedbackManager {
    friend class CStorage;
private:
    VisitorManager& visitors;
    const TicketManager& tickets;
    vector<Feedback> feedbacks;
    string lasterror;
    /// 按反馈编号查找下标
    int findfeedback(const string& feedbackid) const;
    /// 生成未使用的反馈编号
    string generateid() const;
    /// 检查当前游客账号是否有效
    bool checkvisitor(const Session& session);
public:
    /// 关联游客与票务管理器
    FeedbackManager(VisitorManager& visitormanager,
        const TicketManager& ticketmanager);
    /// 提交意见建议
    bool submitsuggestion(const Session& session,
        const string& content,
        string& feedbackid);
    /// 对实际入园的日期提交评分与评价
    bool submitreview(const Session& session,
        const string& visitdate,
        int rating,
        const string& content,
        string& feedbackid);
    /// 游客查询本人反馈，管理员按条件查询全部反馈
    bool getfeedbacks(const Session& session,
        const FeedbackQuery& query,
        vector<Feedback>& result);
    /// 查询单条反馈
    bool getfeedbackdetail(const Session& session,
        const string& feedbackid,
        Feedback& result);
    /// 管理员更新处理状态与回复
    bool handlefeedback(const Session& session,
        const string& feedbackid,
        FeedbackStatus status,
        const string& reply);
    /// 加载反馈文件
    bool loadfromfile(const string& filename);
    /// 保存反馈文件
    bool savetofile(const string& filename);
    /// 获取最近一次操作的错误说明
    string getlasterror() const;
};