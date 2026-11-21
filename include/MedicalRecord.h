#ifndef MEDICALRECORD_H
#define MEDICALRECORD_H
#include <string>
// 抽象类不能作为 vector<MedicalRecord> 的元素，查询改用这个值类型。
/**
 * @brief 承载医疗记录查询副本，避免抽象类按值实例化和对象切片。
 * @note recordId/animalId关联历史；date为日期，veterinarianId为兽医编号，type为类型编码，description为描述。
 */
struct MedicalRecordInfo {
    std::string recordId, animalId, date, veterinarianId, type, description;
};
/**
 * @brief 抽象医疗基类；保存记录编号、动物编号、日期、兽医编号和描述。构造校验字段，toInfo 生成副本；HealthCheckRecord、DiseaseRecord、TreatmentRecord、RecoveryRecord 公有派生并实现 getType。
 */
class MedicalRecord {
  public:
    /**
     * @brief 初始化医疗公共字段并校验格式。
     * @param recordId 唯一医疗编号，例如M001；唯一性由HealthManager检查。
     * @param animalId 关联动物编号；存在性由HealthManager检查。
     * @param date 真实的YYYY-MM-DD日期，检查闰年。
     * @param veterinarianId 非空兽医编号；本模块只校验格式。
     * @param description 非空记录描述，不允许 |、CR、LF。
     * @throws std::invalid_argument 编号、日期或描述不合法。
     */
    MedicalRecord(const std::string &recordId, const std::string &animalId, const std::string &date,
                  const std::string &veterinarianId, const std::string &description);
    /**
     * @brief 通过基类指针安全销毁派生医疗记录。
     */
    virtual ~MedicalRecord() = default;
    /**
     * @brief 由派生记录提供稳定的英文类型编码。
     * @return CHECK/DISEASE/TREATMENT/RECOVERY之一。
     * @details 派生类返回固定医疗类型编码，基类通过虚函数分派获取实际类型。
     */
    virtual std::string getType() const = 0;
    /**
     * @brief 复制当前记录，供调用方独立查询。
     * @return 包含派生类型编码的MedicalRecordInfo副本。
     * @details 复制公共字段，并调用虚函数 getType() 填入实际记录类型。
     */
    MedicalRecordInfo toInfo() const;
    /**
     * @brief 读取医疗记录唯一编号。
     * @return 内部编号字符串的只读引用；对象销毁后不可再用。
     */
    const std::string &getId() const {
        return recordId;
    }
    /**
     * @brief 读取关联动物编号。
     * @return 内部动物编号的只读引用；对象销毁后不可再用。
     */
    const std::string &getAnimalId() const {
        return animalId;
    }

  protected:
    std::string recordId, animalId, date, veterinarianId, description;
};
#endif
