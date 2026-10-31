#include "Bird.h"

/**
 * @brief 通过初始化列表初始化成员或绑定协作对象；实体构造复用字段校验，派生动物委托 Animal 构造。
 * @param id 待处理对象编号。
 * @param name 名称。
 * @param species 物种。
 * @param age 非负年龄。
 * @param gender 性别文本。
 * @param zoneId 所属园区编号。
 * @param status 动物状态。
 */
Bird::Bird(const std::string& id, const std::string& name,
                 const std::string& species, int age, const std::string& gender,
                 const std::string& zoneId, AnimalStatus status)
    : Animal(id, name, species, age, gender, zoneId, status) {}

// override 的实现让基类指针也能得到实际对象的类别。
/**
 * @brief 返回当前派生类对应的固定类别名称，供多态查询及持久化使用。
 * @note 参数：无。
 */
std::string Bird::getCategory() const { return "Bird"; }
