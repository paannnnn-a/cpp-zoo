#ifndef ANIMALCOMMON_H
#define ANIMALCOMMON_H

#include <string>

/*文件说明：定义统一的公共数据*/

// 动物的状态：正常、治疗中、已隔离、转移、已死亡。
enum class AnimalStatus {
    NORMAL, TREATMENT, ISOLATED, TRANSFERRED, DECEASED
};

// 这是查询结果的副本；修改副本不会修改 A 的内部动物对象。
/**
 * @brief 无继承关系的查询值类型，无成员方法；各字段保存对应业务信息，修改副本不会修改管理器内的对象。
 */
struct AnimalBasicInfo {
    std::string animalId;
    std::string name;
    std::string category;
    std::string species;
    std::string zoneId;
    std::string zoneName;
    AnimalStatus status = AnimalStatus::NORMAL;
    bool onDisplay = false;
};

/**
 * @brief 无继承关系的查询值类型，无成员方法；各字段保存对应业务信息，修改副本不会修改管理器内的对象。
 */
struct ZoneInfo {
    std::string zoneId;
    std::string zoneName;
    std::string description;
};

// 用于控制台显示；文件中的状态编码由后续持久化阶段统一实现。
/**
 * @brief 按枚举分支返回中文状态名称；非法值返回未知状态文字。
 * @param status 动物状态。
 */
std::string animalStatusToString(AnimalStatus status);

// 保存文件：状态枚举 → 固定英文编码。非法枚举返回空字符串。
/**
 * @brief 按枚举分支返回固定英文编码；非法值返回空字符串。
 * @param status 动物状态。
 */
std::string animalStatusToCode(AnimalStatus status);

// 加载文件：固定英文编码 → 状态枚举。失败返回 false，outStatus 保持原值。
/**
 * @brief 逐项比较固定英文编码，先解析到局部变量，仅成功后更新输出状态。
 * @param code 固定英文状态编码。
 * @param outStatus 成功时写入的状态；失败保持原值。
 */
bool animalStatusFromCode(const std::string& code,AnimalStatus& outStatus);

#endif
