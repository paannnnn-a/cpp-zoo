#include "visitor/routemanager.h"
#include "ZoneManager.h"
#include "AnimalManager.h"
using namespace std;
RouteManager::RouteManager(const ZoneManager& zonemanager,
    const AnimalManager& animalmanager)
    : zones(zonemanager), animals(animalmanager) {
    Route first;
    first.routeid = "RT001";
    first.name = "熊猫与飞鸟";
    first.description = "参观熊猫馆和鸟类馆，了解大熊猫与鸟类";
    first.zoneids.push_back("Z001");
    first.zoneids.push_back("Z003");
    routes.push_back(first);
    Route second;
    second.routeid = "RT002";
    second.name = "鸟类与爬行动物";
    second.description = "参观鸟类馆和爬行动物馆，认识不同类别的动物";
    second.zoneids.push_back("Z003");
    second.zoneids.push_back("Z004");
    routes.push_back(second);
    Route third;
    third.routeid = "RT003";
    third.name = "全园浏览";
    third.description = "依次参观熊猫馆、虎山、鸟类馆和爬行动物馆";
    third.zoneids.push_back("Z001");
    third.zoneids.push_back("Z002");
    third.zoneids.push_back("Z003");
    third.zoneids.push_back("Z004");
    routes.push_back(third);
}
int RouteManager::findroute(const string& routeid) const {
    for (size_t i = 0; i < routes.size(); i++) {
        if (routes[i].routeid == routeid) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
vector<Route> RouteManager::getroutes(const string& keyword) const {
    vector<Route> result;
    for (size_t i = 0; i < routes.size(); i++) {
        if (keyword.empty()
            || routes[i].name.find(keyword) != string::npos
            || routes[i].description.find(keyword) != string::npos) {
            result.push_back(routes[i]);
        }
    }
    return result;
}
bool RouteManager::getroutedetail(const string& routeid,
    RouteDetail& detail) {
    lasterror.clear();
    int index = findroute(routeid);
    if (index == -1) {
        lasterror = "路线不存在";
        return false;
    }
    RouteDetail result;
    result.route = routes[index];
    for (size_t i = 0; i < result.route.zoneids.size(); i++) {
        const string& zoneid = result.route.zoneids[i];
        RouteStop stop;
        if (!zones.getZoneInfo(zoneid, stop.zone)) {
            result.skippedzoneids.push_back(zoneid);
            continue;
        }
        vector<AnimalBasicInfo> zoneanimals =
            animals.getAnimalsByZone(zoneid);
        // 收集当前正在展出的动物
        for (size_t j = 0; j < zoneanimals.size(); j++) {
            if (zoneanimals[j].onDisplay) {
                stop.animals.push_back(zoneanimals[j]);
            }
        }
        if (stop.animals.empty()) {
            result.skippedzoneids.push_back(zoneid);
            continue;
        }
        result.stops.push_back(stop);
    }
    if (result.stops.empty()) {
        lasterror = "该路线当前没有可参观的动物，请选择其他路线";
        return false;
    }
    detail = result;
    return true;
}
string RouteManager::getlasterror() const {
    return lasterror;
}
