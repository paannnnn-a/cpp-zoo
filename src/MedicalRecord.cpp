#include "MedicalRecord.h"
#include "detail/TextIO.h"
/**
 * @brief 初始化医疗公共字段并校验格式。
 * @param id 唯一医疗编号，例如M001；唯一性由HealthManager检查。
 * @param a 关联动物编号；存在性由HealthManager检查。
 * @param d 真实的YYYY-MM-DD日期，检查闰年。
 * @param vet 非空兽医编号；本模块只校验格式。
 * @param desc 非空记录描述，不允许 |、CR、LF。
 * @throws std::invalid_argument 编号、日期或描述不合法。
 */
MedicalRecord::MedicalRecord(const std::string &id, const std::string &a, const std::string &d,
                             const std::string &vet, const std::string &desc)
    : recordId(id), animalId(a), date(d), veterinarianId(vet), description(desc) {
    zoo_detail::requireId(id, "RecordID");
    zoo_detail::requireId(a, "AnimalID");
    zoo_detail::requireId(vet, "VetID");
    zoo_detail::requireText(desc, "Description");
    if (!zoo_detail::date(d))
        throw std::invalid_argument("日期必须为真实的 YYYY-MM-DD");
}
/**
 * @brief 复制当前记录，供调用方独立查询。
 * @return 包含派生类型编码的MedicalRecordInfo副本。
 * @details 复制公共字段，并调用虚函数 getType() 填入实际记录类型。
 */
MedicalRecordInfo MedicalRecord::toInfo() const {
    return {recordId, animalId, date, veterinarianId, getType(), description};
}
