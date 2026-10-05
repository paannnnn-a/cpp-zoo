#ifndef REPTILE_H
#define REPTILE_H

#include "Animal.h"

// 具体物种用 species 表示，不需要为每个物种重复建立子类。
class Reptile : public Animal {
public:
    Reptile(const std::string& animalId, const std::string& name,
             const std::string& species, int age, const std::string& gender,
             const std::string& zoneId, AnimalStatus status = AnimalStatus::NORMAL);
    std::string getCategory() const override;
};
#endif
