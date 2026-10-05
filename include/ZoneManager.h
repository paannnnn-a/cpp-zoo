#ifndef ZONEMANAGER_H
#define ZONEMANAGER_H

#include <string>
#include <vector>
#include "AnimalCommon.h"
#include "Zone.h"

class ZoneManager {
public:
    // A 管理员录入：重复编号返回 false，不改变原列表。
    bool addZone(const Zone& zone);
    bool zoneExists(const std::string& zoneId) const;
    std::vector<ZoneInfo> getAllZones() const;
    // 不存在时返回 false，outInfo 保持原值。
    bool getZoneInfo(const std::string& zoneId, ZoneInfo& outInfo) const;

    // 加载成功整体替换列表；文件缺失/损坏返回 false，保留旧列表。
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;
    // 普通失败仍按统一规则返回 bool；错误详情仅供 A 显示。
    const std::string& getLastError() const;

private:
    std::vector<Zone> zones;
    // const 保存方法也能记录 I/O 错误，不影响业务数据。
    mutable std::string lastError;
};
#endif
