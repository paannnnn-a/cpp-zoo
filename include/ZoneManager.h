#ifndef ZONEMANAGER_H
#define ZONEMANAGER_H

#include <string>
#include <vector>
#include "AnimalCommon.h"
#include "Zone.h"

/**
 * @brief 无继承关系的园区管理器；zones 保存园区值对象，lastError 保存错误。方法负责新增、查找、生成查询副本及文件读写。
 */
class ZoneManager {
public:
    // A 管理员录入：重复编号返回 false，不改变原列表。
    /**
     * @brief 检查编号是否重复，重复时记录错误，否则将园区副本追加到容器。
     * @param zone 待登记园区。
         */
    bool addZone(const Zone& zone);
    /**
     * @brief 遍历园区并比较编号，命中返回 true，遍历结束返回 false。
     * @param zoneId 所属园区编号。
         */
    bool zoneExists(const std::string& zoneId) const;
    /**
     * @brief 预留容器容量，逐个调用 toInfo 并收集园区副本。
     * @note 参数：无。
         */
    std::vector<ZoneInfo> getAllZones() const;
    // 不存在时返回 false，outInfo 保持原值。
    /**
     * @brief 按编号遍历园区，命中时写入副本并返回 true，否则保持输出参数不变。
     * @param zoneId 所属园区编号。
     * @param outInfo 成功时写入的查询副本；失败保持原值。
         */
    bool getZoneInfo(const std::string& zoneId, ZoneInfo& outInfo) const;

    // 加载成功整体替换列表；文件缺失/损坏返回 false，保留旧列表。
    /**
     * @brief 读取并拆分文件字段，校验格式、编号唯一性及关联对象，先存入临时容器，全部成功后交换正式数据。
     * @param filename 读写文件路径。
         */
    bool loadFromFile(const std::string& filename);
    /**
     * @brief 按固定字段顺序和竖线分隔符序列化记录，检查文件写入结果并返回是否成功。
     * @param filename 读写文件路径。
         */
    bool saveToFile(const std::string& filename) const;
    // 普通失败仍按统一规则返回 bool；错误详情仅供 A 显示。
    const std::string& getLastError() const;

private:
    std::vector<Zone> zones;
    // const 保存方法也能记录 I/O 错误，不影响业务数据。
    mutable std::string lastError;
};
#endif
