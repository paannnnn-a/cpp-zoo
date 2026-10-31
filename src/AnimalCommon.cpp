#include "AnimalCommon.h"

//用于显示中文给人看；参数为状态。
/**
 * @brief 按枚举分支返回中文状态名称；非法值返回未知状态文字。
 * @param status 动物状态。
 */
std::string animalStatusToString(AnimalStatus status) {
    switch (status) {
        case AnimalStatus::NORMAL: return "正常";
        case AnimalStatus::TREATMENT: return "治疗中";
        case AnimalStatus::ISOLATED: return "隔离";
        case AnimalStatus::TRANSFERRED: return "已转出";
        case AnimalStatus::DECEASED: return "已死亡";
    }
    return "未知状态"; // 防止外部强制类型转换得到非法枚举时无返回值。
}

//把 AnimalStatus 枚举值转换成对应的字符串代码；参数：要转换的枚举值。
/**
 * @brief 按枚举分支返回固定英文编码；非法值返回空字符串。
 * @param status 动物状态。
 */
std::string animalStatusToCode(AnimalStatus status) {
    switch (status) {
    case AnimalStatus::NORMAL:
        return "NORMAL";

    case AnimalStatus::TREATMENT:
        return "TREATMENT";

    case AnimalStatus::ISOLATED:
        return "ISOLATED";

    case AnimalStatus::TRANSFERRED:
        return "TRANSFERRED";

    case AnimalStatus::DECEASED:
        return "DECEASED";
    }

    return "";
}

//把字符串代码解析回 AnimalStatus 值；参数：字符串代码、解析成功时写入对应的值。
/**
 * @brief 逐项比较固定英文编码，先解析到局部变量，仅成功后更新输出状态。
 * @param code 固定英文状态编码。
 * @param outStatus 成功时写入的状态；失败保持原值。
 */
bool animalStatusFromCode(
    const std::string& code,
    AnimalStatus& outStatus
) {
    AnimalStatus parsedStatus;

    if (code == "NORMAL") {
        parsedStatus = AnimalStatus::NORMAL;
    }
    else if (code == "TREATMENT") {
        parsedStatus = AnimalStatus::TREATMENT;
    }
    else if (code == "ISOLATED") {
        parsedStatus = AnimalStatus::ISOLATED;
    }
    else if (code == "TRANSFERRED") {
        parsedStatus = AnimalStatus::TRANSFERRED;
    }
    else if (code == "DECEASED") {
        parsedStatus = AnimalStatus::DECEASED;
    }
    else {
        return false;
    }

    outStatus = parsedStatus;
    return true;
}