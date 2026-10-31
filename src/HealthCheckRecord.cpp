#include "HealthCheckRecord.h"
/**
 * @brief 返回体检记录的固定类型编码。
 * @note 参数：无。
 * @return 固定字符串“CHECK”。
 * @details 派生类返回固定医疗类型编码，基类通过虚函数分派获取实际类型。
 */
std::string HealthCheckRecord::getType() const {
    return "CHECK";
}
