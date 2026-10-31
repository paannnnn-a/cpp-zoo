#include "Animal.h"
#include "detail/FieldValidation.h"

/**
 * @brief 通过初始化列表初始化成员或绑定协作对象；实体构造复用字段校验，派生动物委托 Animal 构造。
 * @param id 待处理对象编号。
 * @param animalName 动物名称。
 * @param animalSpecies 动物物种。
 * @param animalAge 动物年龄。
 * @param animalGender 动物性别。
 * @param animalZone 所属园区编号。
 * @param animalStatus 动物状态。
 */
Animal::Animal(const std::string& id, const std::string& animalName,
               const std::string& animalSpecies, int animalAge,
               const std::string& animalGender, const std::string& animalZone,
               AnimalStatus animalStatus)
    : animalId(id), name(animalName), species(animalSpecies), age(animalAge),
      gender(animalGender), zoneId(animalZone), status(animalStatus) {
    zoo_detail::requireId(animalId, "AnimalID");
    // 复用 Setter 校验，保证创建对象与后续修改遵循同样的规则。
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param animalName 动物名称。
         */
    setName(animalName);
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param animalSpecies 动物物种。
         */
    setSpecies(animalSpecies);
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param animalAge 动物年龄。
         */
    setAge(animalAge);
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param animalGender 动物性别。
         */
    setGender(animalGender);
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param animalZone 所属园区编号。
         */
    setZoneId(animalZone);
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param animalStatus 动物状态。
         */
    setStatus(animalStatus);
}

const std::string& Animal::getId() const { return animalId; }
const std::string& Animal::getName() const { return name; }
const std::string& Animal::getSpecies() const { return species; }
/**
 * @brief 直接返回对应成员的值或只读引用；引用的有效期受当前对象生命周期限制。
 * @note 参数：无。
 */
int Animal::getAge() const { return age; }
const std::string& Animal::getGender() const { return gender; }
const std::string& Animal::getZoneId() const { return zoneId; }
/**
 * @brief 直接返回对应成员的值或只读引用；引用的有效期受当前对象生命周期限制。
 * @note 参数：无。
 */
AnimalStatus Animal::getStatus() const { return status; }

/**
 * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
 * @param value 待写入的对应字段值。
 */
void Animal::setName(const std::string& value) {
    zoo_detail::requireText(value, "Name");
    name = value;
}
/**
 * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
 * @param value 待写入的对应字段值。
 */
void Animal::setSpecies(const std::string& value) {
    zoo_detail::requireText(value, "Species");
    species = value;
}
/**
 * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
 * @param value 待写入的对应字段值。
 */
void Animal::setAge(int value) {
    // 文档未规定统一上限；只拒绝负数，不自行设定寿命上限。
    if (value < 0) throw std::invalid_argument("Age：年龄不能为负数");
    age = value;
}
/**
 * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
 * @param value 待写入的对应字段值。
 */
void Animal::setGender(const std::string& value) {
    // 文档没有规定性别编码，允许“雄”“雌”“未知”等合法文本。
    zoo_detail::requireText(value, "Gender");
    gender = value;
}
/**
 * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
 * @param value 待写入的对应字段值。
 */
void Animal::setZoneId(const std::string& value) {
    zoo_detail::requireId(value, "ZoneID");
    zoneId = value;
}
/**
 * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
 * @param value 待写入的对应字段值。
 */
void Animal::setStatus(AnimalStatus value) {
    if (!zoo_detail::validStatus(value)) throw std::invalid_argument("Status：非法动物状态");
    status = value;
}
