#include "Reptile.h"

/**
 * @brief 由 Animal 基类初始化并校验动物的公共字段。
 * @param id 动物编号。
 * @param name 名称。
 * @param species 物种。
 * @param age 非负年龄。
 * @param gender 性别文本。
 * @param zoneId 所属园区编号。
 * @param status 动物状态。
 */
Reptile::Reptile(const std::string& id, const std::string& name,
                 const std::string& species, int age, const std::string& gender,
                 const std::string& zoneId, AnimalStatus status)
    : Animal(id, name, species, age, gender, zoneId, status) {}

// override 的实现让基类指针也能得到实际对象的类别。
/**
 * @brief 返回类别编码 "Reptile"。
 */
std::string Reptile::getCategory() const { return "Reptile"; }
