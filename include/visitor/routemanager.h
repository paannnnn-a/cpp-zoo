#pragma once
#include <string>
#include <vector>
#include "AnimalCommon.h"
using namespace std;
class ZoneManager;
class AnimalManager;
/// 预设游览路线
struct Route {
    string routeid;           ///< 路线编号
    string name;              ///< 路线名称
    string description;       ///< 路线介绍
    vector<string> zoneids;   ///< 按游览顺序排列的园区编号
};
/// 路线中的一个游览站点
struct RouteStop {
    ZoneInfo zone;                    ///< 园区信息
    vector<AnimalBasicInfo> animals;   ///< 正在展出的动物
};
/// 路线的实际查询结果
struct RouteDetail {
    Route route;                       ///< 预设路线
    vector<RouteStop> stops;            ///< 本次可游览的站点
    vector<string> skippedzoneids;      ///< 本次跳过的园区编号
};
/// 管理预设路线并查询可游览站点
class RouteManager {
private:
    const ZoneManager& zones;
    const AnimalManager& animals;
    vector<Route> routes;
    string lasterror;
    /// 按路线编号查找下标
    int findroute(const string& routeid) const;
public:
    /// 关联园区、动物管理器并建立预设路线
    RouteManager(const ZoneManager& zonemanager,
        const AnimalManager& animalmanager);
    /// 按名称或介绍查找路线，空关键词返回全部路线
    vector<Route> getroutes(const string& keyword = "") const;
    /// 查询路线站点及正在展出的动物
    bool getroutedetail(const string& routeid, RouteDetail& detail);
    /// 获取最近一次操作的错误说明
    string getlasterror() const;
};