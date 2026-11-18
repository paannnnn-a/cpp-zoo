#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "ticket.h"
#include "session.h"
using namespace std;
/// 票种查询结果
struct TicketInfo {
    TicketType type = TicketType::adult;  ///< 票种
    string name;                         ///< 名称
    Money price = 0;                     ///< 当前价格，单位为分
    bool onsale = false;                 ///< 是否正在售票
    bool qualification = false;          ///< 是否需要确认优惠资格
    string eligibility;                  ///< 适用资格说明
};
/// 一条节日优惠日期规则
struct HolidayRule {
    int year = 0;   ///< 年份，0表示每年
    int month = 0;  ///< 月份
    int day = 0;    ///< 日期
};
/// 管理票种查询、调价、上下架和文件读写
class TicketCatalog {
    friend class CStorage;
private:
    vector<HolidayRule> holidays;  ///< 节日优惠规则
    /// 查找完全相同的节日规则，未找到返回-1
    int findholiday(int year, int month, int day) const;
    /// 检查节日规则的年月日是否合法
    bool validholiday(int year, int month, int day) const;
    AdultTicket adult;      ///< 成人票
    StudentTicket student;  ///< 学生票
    ChildTicket child;      ///< 儿童票
    string lasterror;       ///< 最近一次操作的错误说明
    /// 查找可修改的票种对象
    Ticket* findticket(TicketType type);
    /// 查找只读的票种对象
    const Ticket* findticket(TicketType type) const;
public:
    /// 创建三种票，使用各自的默认价格和售票状态
    TicketCatalog();
    /**
     * @brief 查询指定票种
     * @param info 成功时接收票种资料
     * @return 找到票种返回 true
     */
    bool getticketinfo(TicketType type, TicketInfo& info);
    /// 获取全部票种，包括已下架票种
    vector<TicketInfo> getallticketinfo() const;
    /// 管理员设置大于零的票价
    bool updateprice(const Session& session,
        TicketType type,
        Money price);
    /// 管理员设置票种上下架状态
    bool setonsale(const Session& session,
        TicketType type,
        bool onsale);
    /// 管理员添加节日规则
    bool addholiday(const Session& session,
        int year, int month, int day);
    /// 管理员删除指定节日规则
    bool removeholiday(const Session& session,
        int year, int month, int day);
    /// 管理员查询全部节日规则
    bool getholidays(const Session& session,
        vector<HolidayRule>& result);
    /// 判断指定游玩日期是否享受节日优惠
    bool isholiday(const string& date) const;
    /// 保存节日规则
    bool saveholidaystofile(const string& filename);
    /// 加载节日规则
    bool loadholidaysfromfile(const string& filename);
    /// 加载三种票的价格和售票状态
    bool loadfromfile(const string& filename);
    /// 保存三种票的价格和售票状态
    bool savetofile(const string& filename);
    /// 获取最近一次操作的错误说明
    string getlasterror() const;
};
