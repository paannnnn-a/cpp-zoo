#ifndef ZONE_H
#define ZONE_H

#include <string>
#include "AnimalCommon.h"

class Zone {
public:
    Zone(const std::string& zoneId, const std::string& zoneName,
         const std::string& description = "");
    const std::string& getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    void setName(const std::string& zoneName);
    void setDescription(const std::string& description);
    ZoneInfo toInfo() const;

private:
    // 无编号 Setter；Manager 也不暴露内部 Zone 的可写引用。
    std::string zoneId;
    std::string zoneName;
    std::string description;
};
#endif
