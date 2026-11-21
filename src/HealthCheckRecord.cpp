#include "HealthCheckRecord.h"
/**
 * @brief 返回体检记录的固定类型编码。
 * @return 固定字符串“CHECK”。
 */
std::string HealthCheckRecord::getType() const {
    return "CHECK";
}
