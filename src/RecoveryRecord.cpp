#include "RecoveryRecord.h"
/**
 * @brief 返回康复记录的固定类型编码。
 * @return 固定字符串“RECOVERY”。
 */
std::string RecoveryRecord::getType() const {
    return "RECOVERY";
}
