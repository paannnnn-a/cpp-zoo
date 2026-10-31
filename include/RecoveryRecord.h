#pragma once
#include "MedicalRecord.h"
// 复用基类构造函数，派生类负责区分业务类型。
/**
 * @brief 表示一条康复记录，复用医疗基类的公共字段及校验。
 * @note 类型固定为RECOVERY；新增记录是否改变动物状态由HealthManager决定。
 */
/**
 * @brief 公有继承 MedicalRecord，复用记录编号、动物编号、日期、兽医和描述，不新增字段；继承基类构造函数，重写 getType 区分医疗业务。
 */
class RecoveryRecord : public MedicalRecord {
  public:
    /**
     * @brief 继承医疗基类的五参数构造函数。
     * @param recordId 唯一医疗编号，例如M001；唯一性由HealthManager检查。
     * @param animalId 关联动物编号；存在性由HealthManager检查。
     * @param date 真实的YYYY-MM-DD日期，检查闰年。
     * @param veterinarianId 非空兽医编号；本模块只校验格式。
     * @param description 非空记录描述，不允许 |、CR、LF。
     * @note 无返回值；构造仅检查字段，不自动登记、不自动变更状态。
     */
    using MedicalRecord::MedicalRecord;
    /**
     * @brief 返回康复记录的固定类型编码。
     * @note 参数：无。
     * @return 固定字符串“RECOVERY”，用于文件保存和界面识别。
     * @details 派生类返回固定医疗类型编码，基类通过虚函数分派获取实际类型。
 */
    std::string getType() const override;
};
