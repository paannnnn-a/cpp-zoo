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
     * @brief 通过初始化列表初始化成员或绑定协作对象；实体构造复用字段校验，派生动物委托 Animal 构造。
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
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param zoneName 园区名称。
         */
    void setName(const std::string& zoneName);
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param description 描述文本。
         */
    void setDescription(const std::string& description);
    /**
     * @brief 按成员顺序构造值类型查询副本，医疗记录通过 getType 获取派生类型。
     * @note 参数：无。
         */
    ZoneInfo toInfo() const;

private:
    // 无编号 Setter；Manager 也不暴露内部 Zone 的可写引用。
    std::string zoneId;
    std::string zoneName;
    std::string description;
};
#endif
