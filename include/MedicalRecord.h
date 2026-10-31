#ifndef MEDICALRECORD_H
#define MEDICALRECORD_H
#include <string>
// 抽象类不能作为 vector<MedicalRecord> 的元素，查询改用这个值类型。
/**
 * @brief 承载医疗记录查询副本，避免抽象类按值实例化和对象切片。
 * @note recordId/animalId关联历史；date为日期，veterinarianId为兽医编号，type为类型编码，description为描述。
 */
/**
 * @brief 无继承关系的查询值类型，无成员方法；各字段保存对应业务信息，修改副本不会修改管理器内的对象。
 */
struct MedicalRecordInfo {
    std::string recordId, animalId, date, veterinarianId, type, description;
};
/**
 * @brief 保存医疗记录的公共字段，并由派生类确定业务类型。
 * @note 通过虚析构支持基类unique_ptr安全销毁；构造时校验编号、日期和描述。
 */
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
     * @note 构造函数无返回值；非法字段抛出std::invalid_argument。
     * @details 通过初始化列表初始化成员或绑定协作对象；实体构造复用字段校验，派生动物委托 Animal 构造。
 */
    MedicalRecord(const std::string &recordId, const std::string &animalId, const std::string &date,
                  const std::string &veterinarianId, const std::string &description);
    /**
     * @brief 通过基类指针安全销毁派生医疗记录。
     * @note 参数：无。
     * @return 无。
     * @note 虚析构，无需手动释放成员字符串。
     * @details 使用默认析构释放成员资源；虚析构同时执行派生类析构。
 */
    virtual ~MedicalRecord() = default;
    /**
     * @brief 由派生记录提供稳定的英文类型编码。
     * @note 参数：无。
     * @return CHECK/DISEASE/TREATMENT/RECOVERY之一。
     * @details 派生类返回固定医疗类型编码，基类通过虚函数分派获取实际类型。
 */
    virtual std::string getType() const = 0;
    /**
     * @brief 将当前多态对象转换为只读查询副本。
     * @note 参数：无。
     * @return 包含派生类型编码的MedicalRecordInfo副本。
     * @details 按成员顺序构造值类型查询副本，医疗记录通过 getType 获取派生类型。
 */
    MedicalRecordInfo toInfo() const;
    /**
     * @brief 读取医疗记录唯一编号。
     * @note 参数：无。
     * @return 内部编号字符串的只读引用；对象销毁后不可再用。
     */
    const std::string &getId() const {
        return recordId;
    }
    /**
     * @brief 读取关联动物编号。
     * @note 参数：无。
     * @return 内部动物编号的只读引用；对象销毁后不可再用。
     */
    const std::string &getAnimalId() const {
        return animalId;
    }

  protected:
    std::string recordId, animalId, date, veterinarianId, description;
};
#endif
