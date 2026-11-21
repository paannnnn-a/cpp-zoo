#ifndef ZONE_H
#define ZONE_H

#include <string>
#include "AnimalCommon.h"

/**
 * @brief 无继承关系的园区实体；zoneId 为编号，zoneName 为名称，description 为描述。Getter 读取字段，Setter 校验修改，toInfo 返回副本。
 */
class Zone {
public:
    /**
     * @brief 校验园区编号和名称，描述允许留空。
     * @param zoneId 所属园区编号。
     * @param zoneName 园区名称。
     * @param description 描述文本。
     */
    Zone(const std::string& zoneId, const std::string& zoneName,
         const std::string& description = "");
    const std::string& getId() const;
    const std::string& getName() const;
    const std::string& getDescription() const;
    /**
     * @brief 名称须包含非空白字符，且不能包含竖线或换行。
     * @throws std::invalid_argument 字段校验失败。
     */
    void setName(const std::string& zoneName);
    /**
     * @brief 描述允许留空，但不能包含竖线或换行。
     * @throws std::invalid_argument 字段校验失败。
     */
    void setDescription(const std::string& description);
    /**
     * @brief 返回包含园区编号、名称和描述的 ZoneInfo 副本。
     */
    ZoneInfo toInfo() const;

private:
    // 无编号 Setter；Manager 也不暴露内部 Zone 的可写引用。
    std::string zoneId;
    std::string zoneName;
    std::string description;
};
#endif
