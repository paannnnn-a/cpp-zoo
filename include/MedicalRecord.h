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
 * @brief 保存医疗记录的公共字段，并由派生类确定业务类型。
 * @note 通过虚析构支持基类unique_ptr安全销毁；构造时校验编号、日期和描述。
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
     * @note 构造函数无返回值；非法字段抛出std::invalid_argument。
     */
    MedicalRecord(const std::string &recordId, const std::string &animalId, const std::string &date,
                  const std::string &veterinarianId, const std::string &description);
    /**
     * @brief 通过基类指针安全销毁派生医疗记录。
     * 参数：无。
     * @return 无。
     * @note 虚析构，无需手动释放成员字符串。
     */
    virtual ~MedicalRecord() = default;
    /**
     * @brief 由派生记录提供稳定的英文类型编码。
     * 参数：无。
     * @return CHECK/DISEASE/TREATMENT/RECOVERY之一。
     */
    virtual std::string getType() const = 0;
    /**
     * @brief 将当前多态对象转换为只读查询副本。
     * 参数：无。
     * @return 包含派生类型编码的MedicalRecordInfo副本。
     */
    MedicalRecordInfo toInfo() const;
    /**
     * @brief 读取医疗记录唯一编号。
     * 参数：无。
     * @return 内部编号字符串的只读引用；对象销毁后不可再用。
     */
    const std::string &getId() const {
        return recordId;
    }
    /**
     * @brief 读取关联动物编号。
     * 参数：无。
     * @return 内部动物编号的只读引用；对象销毁后不可再用。
     */
    const std::string &getAnimalId() const {
        return animalId;
    }

  protected:
    std::string recordId, animalId, date, veterinarianId, description;
};
#endif
