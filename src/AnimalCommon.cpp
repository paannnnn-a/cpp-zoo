#include "AnimalCommon.h"

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
