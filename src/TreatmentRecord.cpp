#include "TreatmentRecord.h"
/**
 * @brief 返回治疗记录的固定类型编码。
 * 参数：无。
 * @return 固定字符串“TREATMENT”。
 */
std::string TreatmentRecord::getType() const {
    return "TREATMENT";
}
