#include "Animal.h"
#include "detail/FieldValidation.h"

Animal::Animal(const std::string& id, const std::string& animalName,
               const std::string& animalSpecies, int animalAge,
               const std::string& animalGender, const std::string& animalZone,
               AnimalStatus animalStatus)
    : animalId(id), name(animalName), species(animalSpecies), age(animalAge),
      gender(animalGender), zoneId(animalZone), status(animalStatus) {
    zoo_detail::requireId(animalId, "AnimalID");
    // 复用 Setter 校验，保证创建对象与后续修改遵循同样的规则。
    setName(animalName);
    setSpecies(animalSpecies);
    setAge(animalAge);
    setGender(animalGender);
    setZoneId(animalZone);
    setStatus(animalStatus);
}

const std::string& Animal::getId() const { return animalId; }
const std::string& Animal::getName() const { return name; }
const std::string& Animal::getSpecies() const { return species; }
int Animal::getAge() const { return age; }
const std::string& Animal::getGender() const { return gender; }
const std::string& Animal::getZoneId() const { return zoneId; }
AnimalStatus Animal::getStatus() const { return status; }

void Animal::setName(const std::string& value) {
    zoo_detail::requireText(value, "Name");
    name = value;
}
void Animal::setSpecies(const std::string& value) {
    zoo_detail::requireText(value, "Species");
    species = value;
}
void Animal::setAge(int value) {
    // 文档未规定统一上限；只拒绝负数，不自行设定寿命上限。
    if (value < 0) throw std::invalid_argument("Age：年龄不能为负数");
    age = value;
}
void Animal::setGender(const std::string& value) {
    // 文档没有规定性别编码，允许“雄”“雌”“未知”等合法文本。
    zoo_detail::requireText(value, "Gender");
    gender = value;
}
void Animal::setZoneId(const std::string& value) {
    zoo_detail::requireId(value, "ZoneID");
    zoneId = value;
}
void Animal::setStatus(AnimalStatus value) {
    if (!zoo_detail::validStatus(value)) throw std::invalid_argument("Status：非法动物状态");
    status = value;
}
