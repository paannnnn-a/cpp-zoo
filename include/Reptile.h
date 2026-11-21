#ifndef REPTILE_H
#define REPTILE_H

#include "Animal.h"

// 具体物种用 species 表示，不需要为每个物种重复建立子类。
/**
 * @brief 公有继承 Animal，复用基类全部动物属性，不新增字段；构造函数委托基类校验，重写 getCategory 返回 Reptile。
 */
class Reptile : public Animal {
public:
    /**
     * @brief 由 Animal 基类初始化并校验动物的公共字段。
     * @param animalId 动物唯一编号。
     * @param name 名称。
     * @param species 物种。
     * @param age 非负年龄。
     * @param gender 性别文本。
     * @param zoneId 所属园区编号。
     * @param status 动物状态。
     */
    Reptile(const std::string& animalId, const std::string& name,
             const std::string& species, int age, const std::string& gender,
             const std::string& zoneId, AnimalStatus status = AnimalStatus::NORMAL);
    /**
     * @brief 返回类别编码 "Reptile"。
     */
    std::string getCategory() const override;
};
#endif
