#ifndef ANIMAL_H
#define ANIMAL_H

#include <string>
#include "AnimalCommon.h"

// 抽象基类不能直接创建对象，实际对象应为 Mammal、Bird 或 Reptile。
class Animal {
public:
    Animal(const std::string& animalId, const std::string& name,
           const std::string& species, int age, const std::string& gender,
           const std::string& zoneId, AnimalStatus status = AnimalStatus::NORMAL);
    // 基类指针销毁派生对象时，必须有虚析构函数。
    virtual ~Animal() = default;

    // 禁止整体赋值绕过“不能修改编号”的约定；用 Setter 修改业务字段。
    Animal(const Animal&) = delete;
    Animal& operator=(const Animal&) = delete;
    Animal(Animal&&) = delete;
    Animal& operator=(Animal&&) = delete;

    const std::string& getId() const;
    const std::string& getName() const;
    const std::string& getSpecies() const;
    int getAge() const;
    const std::string& getGender() const;
    const std::string& getZoneId() const;
    AnimalStatus getStatus() const;

    // 构造与 Setter 校验失败抛出 invalid_argument；不会留下非法数据。
    void setName(const std::string& name);
    void setSpecies(const std::string& species);
    void setAge(int age);
    void setGender(const std::string& gender);
    // 此处只校验编号格式。园区是否存在由阶段4的 AnimalManager 校验。
    void setZoneId(const std::string& zoneId);
    void setStatus(AnimalStatus status);

    virtual std::string getCategory() const = 0;

private:
    // 不提供 setAnimalId，避免喂养/医疗历史失去对应关系。
    const std::string animalId;
    std::string name;
    std::string species;
    int age;
    std::string gender;
    std::string zoneId;
    AnimalStatus status;
};
#endif
