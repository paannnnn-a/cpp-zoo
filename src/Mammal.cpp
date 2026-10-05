#include "Mammal.h"

Mammal::Mammal(const std::string& id, const std::string& name,
                 const std::string& species, int age, const std::string& gender,
                 const std::string& zoneId, AnimalStatus status)
    : Animal(id, name, species, age, gender, zoneId, status) {}

// override 的实现让基类指针也能得到实际对象的类别。
std::string Mammal::getCategory() const { return "Mammal"; }
