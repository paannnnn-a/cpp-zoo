#include "Zone.h"
#include "detail/FieldValidation.h"

/**
 * @brief 校验园区编号和名称，描述允许留空。
 * @param id 园区编号。
 * @param name 名称。
 * @param desc 园区描述，可留空。
 */
Zone::Zone(const std::string& id, const std::string& name, const std::string& desc)
    : zoneId(id), zoneName(name), description(desc) {
    zoo_detail::requireId(zoneId, "ZoneID");
    setName(name);
    setDescription(desc);
}
const std::string& Zone::getId() const { return zoneId; }
const std::string& Zone::getName() const { return zoneName; }
const std::string& Zone::getDescription() const { return description; }
void Zone::setName(const std::string& value) {
    zoo_detail::requireText(value, "ZoneName");
    zoneName = value;
}
void Zone::setDescription(const std::string& value) {
    zoo_detail::requireText(value, "Description", false); // 描述允许留空。
    description = value;
}
/**
 * @brief 返回包含园区编号、名称和描述的 ZoneInfo 副本。
 */
ZoneInfo Zone::toInfo() const {
    return {zoneId, zoneName, description}; // 返回副本，C 不能修改内部数据。
}
