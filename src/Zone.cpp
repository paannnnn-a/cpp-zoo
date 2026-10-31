#include "Zone.h"
#include "detail/FieldValidation.h"

/**
 * @brief 通过初始化列表初始化成员或绑定协作对象；实体构造复用字段校验，派生动物委托 Animal 构造。
 * @param id 待处理对象编号。
 * @param name 名称。
 * @param desc 参与本次操作的desc参数。
 */
Zone::Zone(const std::string& id, const std::string& name, const std::string& desc)
    : zoneId(id), zoneName(name), description(desc) {
    zoo_detail::requireId(zoneId, "ZoneID");
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param name 名称。
         */
    setName(name);
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param desc 参与本次操作的desc参数。
         */
    setDescription(desc);
}
const std::string& Zone::getId() const { return zoneId; }
const std::string& Zone::getName() const { return zoneName; }
const std::string& Zone::getDescription() const { return description; }
/**
 * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
 * @param value 待写入的对应字段值。
 */
void Zone::setName(const std::string& value) {
    zoo_detail::requireText(value, "ZoneName");
    zoneName = value;
}
/**
 * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
 * @param value 待写入的对应字段值。
 */
void Zone::setDescription(const std::string& value) {
    zoo_detail::requireText(value, "Description", false); // 描述允许留空。
    description = value;
}
/**
 * @brief 按成员顺序构造值类型查询副本，医疗记录通过 getType 获取派生类型。
 * @note 参数：无。
 */
ZoneInfo Zone::toInfo() const {
    return {zoneId, zoneName, description}; // 返回副本，C 不能修改内部数据。
}
