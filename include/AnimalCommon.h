#ifndef ANIMALCOMMON_H
#define ANIMALCOMMON_H

#include <string>

/*文件说明：定义统一的公共数据*/

// 动物的状态：正常、治疗中、已隔离、转移、已死亡。
enum class AnimalStatus {
    NORMAL, TREATMENT, ISOLATED, TRANSFERRED, DECEASED
};

// 这是查询结果的副本；修改副本不会修改 A 的内部动物对象。
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

struct ZoneInfo {
    std::string zoneId;
    std::string zoneName;
    std::string description;
};

// 用于控制台显示；文件中的状态编码由后续持久化阶段统一实现。
std::string animalStatusToString(AnimalStatus status);

// 保存文件：状态枚举 → 固定英文编码。非法枚举返回空字符串。
std::string animalStatusToCode(AnimalStatus status);

// 加载文件：固定英文编码 → 状态枚举。失败返回 false，outStatus 保持原值。
bool animalStatusFromCode(const std::string& code,AnimalStatus& outStatus);

#endif
